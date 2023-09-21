#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.h"

Texture::Texture(Device &device)
    : m_Device(device)
{
}

Texture::~Texture()
{
}

void Texture::LoadFromFile(const std::string &filename)
{
    int textureWidth, textureHeight, textureChannels;
    stbi_uc* pixels = stbi_load(
		"resources/assets/images/plant.jpg",
		&textureWidth,
		&textureHeight,
		&textureChannels,
		STBI_rgb_alpha
	);
	vk::DeviceSize imageSize = textureWidth * textureHeight * 4;

    if (!pixels)
        throw std::runtime_error("Failed to load texture image");

	vk::DeviceMemory stagingBufferMemory;
	vk::Buffer stagingBuffer = m_Device.CreateBuffer(
		imageSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBufferMemory
	);

	void* data = m_Device.GetDevice().mapMemory(stagingBufferMemory, 0, imageSize, vk::MemoryMapFlags());
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	m_Device.GetDevice().unmapMemory(stagingBufferMemory);

	stbi_image_free(pixels);
	
	m_Image = m_Device.CreateImage(
		textureWidth,
		textureHeight,
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		m_ImageMemory
	);

	TransitionImageLayout(m_Image, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	m_Device.CopyBufferToImage(stagingBuffer, m_Image, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));
	TransitionImageLayout(m_Image, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

	m_Device.GetDevice().destroyBuffer(stagingBuffer);
	m_Device.GetDevice().freeMemory(stagingBufferMemory);

    m_ImageView = m_Device.CreateImageView(m_Image, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
}

void Texture::Destroy()
{
    m_Device.GetDevice().destroyImageView(m_ImageView);
    m_Device.GetDevice().destroyImage(m_Image);
	m_Device.GetDevice().freeMemory(m_ImageMemory);
}

vk::DescriptorImageInfo Texture::DescriptorInfo(vk::Sampler sampler)
{
    return vk::DescriptorImageInfo(
        sampler,
        m_ImageView,
        vk::ImageLayout::eShaderReadOnlyOptimal
    );
}

void Texture::TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::CommandBuffer commandBuffer = m_Device.BeginSingleTimeCommands();

	vk::ImageMemoryBarrier barrier(
		vk::AccessFlags(),
		vk::AccessFlags(),
		oldLayout,
		newLayout,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		image,
		vk::ImageSubresourceRange(
			vk::ImageAspectFlagBits::eColor,
			0, 1,
			0, 1
		)
	);

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlags();
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}

	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}

	else
		throw std::invalid_argument("Unsupported layout transition");

	commandBuffer.pipelineBarrier(
		sourceStage,
		destinationStage,
		vk::DependencyFlags(),
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	m_Device.EndSingleTimeCommands(commandBuffer);
}
