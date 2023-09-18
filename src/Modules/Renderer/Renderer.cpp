#include <iostream>
#include <array>
#include <chrono>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Renderer.h"


Renderer::Renderer(Window* window)
	: m_Window{ window }
{
	std::cout << "Renderer Constructor" << std::endl;
	Resize(m_Window->Width, m_Window->Height);
}

Renderer::~Renderer()
{
	std::cout << "Renderer Destructor" << std::endl;
	delete m_Pipeline;
	delete m_Device;
}

void Renderer::Initialize()
{
	try
	{
		m_Device = new Device(m_Window);
		m_Device->Initialize();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		m_Pipeline = new Pipeline(m_Device->GetDevice(), m_RenderPass, m_DescriptorSetLayout);
		m_Pipeline->Create("resources/shaders/default.vert.spv", "resources/shaders/default.frag.spv", Vertex::GetDescriptions());
		CreateDepthResources();
		CreateFramebuffers();
		CreateTextureImage();
		CreateTextureImageViews();
		CreateTextureSampler();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
		CreateSyncObjects();
	}
	catch (vk::SystemError& err)
    {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        exit(-1);
    }
}

void Renderer::Update()
{
	try
	{
		DrawFrame();
	}
	catch (vk::SystemError& err)
    {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        exit(-1);
    }
}

void Renderer::WaitIdle()
{
	m_Device->WaitIdle();
}

void Renderer::Terminate()
{
	try
	{
		DestroySwapChain();
		DestroyIndexBuffer();
		DestroyVertexBuffer();
		DestroyFramebuffers();
		DestroyImageViews();
		DestroyTextureImageViews();
		DestroyTextureImage();
		DestroyDepthResources();
		DestroyTextureSampler();
		DestroyeDescriptorPool();
		DestroyDescriptorSetLayout();
		m_Pipeline->Terminate();
		DestroyRenderPass();
		DestroyUniformBuffers();
		DestroySyncObjects();
		m_Device->Terminate();
	}
	catch (vk::SystemError& err)
    {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        exit(-1);
    }
}

void Renderer::Resize(uint32_t width, uint32_t height)
{
	if (m_Width == width && m_Height == height) return;
	m_Width = width;
	m_Height = height;
	m_FramebufferResized = true;
}

vk::SurfaceFormatKHR Renderer::SelectSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return availableFormat;

	return availableFormats[0];
}

vk::PresentModeKHR Renderer::SelectSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
		if (availablePresentMode == vk::PresentModeKHR::eMailbox)
			return availablePresentMode;

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Renderer::SelectSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;

	else {
		int width, height;
		glfwGetFramebufferSize(m_Window->GetWindow(), &width, &height);

		vk::Extent2D actualExtent(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void Renderer::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = m_Device->QuerySwapChainSupport(m_Device->GetPhysicalDevice());

	vk::SurfaceFormatKHR surfaceFormat = SelectSwapSurfaceFormat(swapChainSupport.Formats);
	vk::PresentModeKHR presentMode = SelectSwapPresentMode(swapChainSupport.PresentModes);
	vk::Extent2D extent = SelectSwapExtent(swapChainSupport.Capabilities);

	uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;

	if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
		imageCount = swapChainSupport.Capabilities.maxImageCount;

	vk::SwapchainCreateInfoKHR createInfo(
		vk::SwapchainCreateFlagsKHR(),
		m_Device->GetSurface(),
		imageCount,
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment
	);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo( vk::SwapchainCreateFlagsKHR(),
		m_Device->GetSurface(),
		imageCount,
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive
	);

	uint32_t queueFamilyIndices[2] = {
		m_Device->GetQueueFamilies().GraphicsFamily.value(),
		m_Device->GetQueueFamilies().PresentFamily.value()
	};

	if (m_Device->GetQueueFamilies().GraphicsFamily != m_Device->GetQueueFamilies().PresentFamily)
	{
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}

	m_SwapChain = m_Device->GetDevice().createSwapchainKHR(createInfo);

	m_SwapChainImages = m_Device->GetDevice().getSwapchainImagesKHR(m_SwapChain);
	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

void Renderer::DestroySwapChain()
{
	m_Device->GetDevice().destroySwapchainKHR(m_SwapChain);
}

void Renderer::CreateImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		m_SwapChainImageViews[i] = CreateImageView(m_SwapChainImages[i], m_SwapChainImageFormat, vk::ImageAspectFlagBits::eColor);
}

void Renderer::DestroyImageViews()
{
	for (auto imageView : m_SwapChainImageViews)
		m_Device->GetDevice().destroyImageView(imageView);
}

void Renderer::RecreateSwapChain()
{
	while (m_Width == 0 || m_Height == 0)
		glfwWaitEvents();

	m_Device->WaitIdle();

	DestroyFramebuffers();
	DestroyDepthResources();
	DestroyImageViews();
	DestroySwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateDepthResources();
	CreateFramebuffers();
}

void Renderer::CreateRenderPass()
{
	vk::AttachmentDescription colorAttachment(
		vk::AttachmentDescriptionFlags(),
		m_SwapChainImageFormat,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::ePresentSrcKHR
	);

	vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

	vk::AttachmentDescription depthAttachment(
		vk::AttachmentDescriptionFlags(),
		vk::Format::eD32SfloatS8Uint,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eDontCare,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthStencilAttachmentOptimal
	);

	vk::AttachmentReference depthAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription subpass(
		vk::SubpassDescriptionFlags(),
		vk::PipelineBindPoint::eGraphics,
		0,
		nullptr,
		1,
		&colorAttachmentRef,
		nullptr,
		&depthAttachmentRef
	);

	vk::SubpassDependency dependency(
		VK_SUBPASS_EXTERNAL,
		0,
		vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
		vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
		vk::AccessFlagBits::eColorAttachmentRead,
		vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
	);

	vk::AttachmentDescription attachments[] = { colorAttachment, depthAttachment };

	vk::RenderPassCreateInfo renderPassInfo(
		vk::RenderPassCreateFlags(),
		2, attachments,
		1, &subpass,
		1, &dependency
	);

	m_RenderPass = m_Device->GetDevice().createRenderPass(renderPassInfo);
}

void Renderer::DestroyRenderPass()
{
	m_Device->GetDevice().destroyRenderPass(m_RenderPass);
}

void Renderer::CreateFramebuffers()
{
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());
	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
	{
		vk::ImageView attachments[] = {
			m_SwapChainImageViews[i],
			m_DepthImageView
		};

		vk::FramebufferCreateInfo framebufferInfo(
			vk::FramebufferCreateFlags(),
			m_RenderPass,
			2, attachments,
			m_SwapChainExtent.width,
			m_SwapChainExtent.height,
			1
		);

		m_SwapChainFramebuffers[i] = m_Device->GetDevice().createFramebuffer(framebufferInfo);
	}
}

void Renderer::DestroyFramebuffers()
{
	for (auto framebuffer : m_SwapChainFramebuffers)
		m_Device->GetDevice().destroyFramebuffer(framebuffer);
}

void Renderer::CreateDepthResources()
{
	vk::Format depthFormat = vk::Format::eD32SfloatS8Uint;

	CreateImage(
		m_SwapChainExtent.width,
		m_SwapChainExtent.height,
		depthFormat,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		m_DepthImage,
		m_DepthImageMemory
	);

	m_DepthImageView = CreateImageView(m_DepthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

void Renderer::DestroyDepthResources()
{
	m_Device->GetDevice().destroyImageView(m_DepthImageView);
	m_Device->GetDevice().destroyImage(m_DepthImage);
	m_Device->GetDevice().freeMemory(m_DepthImageMemory);
}

void Renderer::CreateVertexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	
	Buffer stagingBuffer = Buffer(
		*m_Device,
		bufferSize,
		1,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
	);
	stagingBuffer.Map();
	stagingBuffer.WriteToBuffer((void *)vertices.data());

	m_VertexBuffer = new Buffer(
		*m_Device,
		bufferSize,
		1,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);

	m_Device->CopyBuffer(stagingBuffer.GetBuffer(), m_VertexBuffer->GetBuffer(), bufferSize);
}

void Renderer::DestroyVertexBuffer()
{
	delete m_VertexBuffer;
}

void Renderer::CreateIndexBuffer()
{
	if (indices.size() <= 0)
		return;

	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	Buffer stagingBuffer = Buffer(
		*m_Device,
		bufferSize,
		1,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
	);

	stagingBuffer.Map();
	stagingBuffer.WriteToBuffer((void *)indices.data());

	m_IndexBuffer = new Buffer(
		*m_Device,
		bufferSize,
		1,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	

	m_Device->CopyBuffer(stagingBuffer.GetBuffer(), m_IndexBuffer->GetBuffer(), bufferSize);
}

void Renderer::DestroyIndexBuffer()
{
	if (indices.size() <= 0)
		return;

	delete m_IndexBuffer;
}

void Renderer::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_UniformBuffers[i] = new Buffer(
			*m_Device,
			bufferSize,
			1,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);
		m_UniformBuffers[i]->Map();
	}
}

void Renderer::DestroyUniformBuffers()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		delete m_UniformBuffers[i];
	}
}

void Renderer::UpdateUniformbuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.Model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.Projection= glm::perspective(glm::radians(45.0f), m_Width / (float)m_Height, 0.1f, 10.0f);

	ubo.Projection[1][1] *= -1;

	m_UniformBuffers[currentImage]->WriteToBuffer(&ubo);
}

void Renderer::CreateTextureImage()
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
	vk::Buffer stagingBuffer = m_Device->CreateBuffer(
		imageSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBufferMemory
	);

	void* data = m_Device->GetDevice().mapMemory(stagingBufferMemory, 0, imageSize, vk::MemoryMapFlags());
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	m_Device->GetDevice().unmapMemory(stagingBufferMemory);

	stbi_image_free(pixels);
	
	CreateImage(
		textureWidth,
		textureHeight,
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		m_TextureImage,
		m_TextureImageMemory
	);

	TransitionImageLayout(m_TextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	CopyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));
	TransitionImageLayout(m_TextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

	m_Device->GetDevice().destroyBuffer(stagingBuffer);
	m_Device->GetDevice().freeMemory(stagingBufferMemory);
}

void Renderer::DestroyTextureImage()
{
	m_Device->GetDevice().destroyImage(m_TextureImage);
	m_Device->GetDevice().freeMemory(m_TextureImageMemory);
}

void Renderer::CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image &image, vk::DeviceMemory &imageMemory)
{
	vk::ImageCreateInfo imageInfo(
		vk::ImageCreateFlags(),
		vk::ImageType::e2D,
		format,
		vk::Extent3D(width, height, 1),
		1,
		1,
		vk::SampleCountFlagBits::e1,
		tiling,
		usage,
		vk::SharingMode::eExclusive,
		0,
		nullptr,
		vk::ImageLayout::eUndefined
	);

	image = m_Device->GetDevice().createImage(imageInfo);

	vk::MemoryRequirements memRequirements = m_Device->GetDevice().getImageMemoryRequirements(image);
	vk::MemoryAllocateInfo allocInfo(
		memRequirements.size,
		m_Device->FindMemoryType(memRequirements.memoryTypeBits, properties)
	);

	vk::Result allocateResult = m_Device->GetDevice().allocateMemory(&allocInfo, nullptr, &imageMemory);
	if (allocateResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate image memory");

	m_Device->GetDevice().bindImageMemory(image, imageMemory, 0);
}

vk::ImageView Renderer::CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
	vk::ImageViewCreateInfo viewInfo(
		vk::ImageViewCreateFlags(),
		image,
		vk::ImageViewType::e2D,
		format,
		vk::ComponentMapping(
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity
		),
		vk::ImageSubresourceRange(
			aspectFlags,
			0, 1,
			0, 1
		)
	);

	return m_Device->GetDevice().createImageView(viewInfo);
}

void Renderer::CreateTextureImageViews()
{
	m_TextureImageView = CreateImageView(m_TextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
}

void Renderer::DestroyTextureImageViews()
{
	m_Device->GetDevice().destroyImageView(m_TextureImageView);
}

void Renderer::TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	vk::CommandBuffer commandBuffer = m_Device->BeginSingleTimeCommands();

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

	m_Device->EndSingleTimeCommands(commandBuffer);
}

void Renderer::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
	vk::CommandBuffer commandBuffer = m_Device->BeginSingleTimeCommands();

	vk::BufferImageCopy region(
		0, 0, 0,
		vk::ImageSubresourceLayers(
			vk::ImageAspectFlagBits::eColor,
			0, 0, 1
		),
		vk::Offset3D(0, 0, 0),
		vk::Extent3D(width, height, 1)
	);

	commandBuffer.copyBufferToImage(
		buffer,
		image,
		vk::ImageLayout::eTransferDstOptimal,
		1,
		&region
	);

	m_Device->EndSingleTimeCommands(commandBuffer);
}

void Renderer::CreateTextureSampler()
{
	vk::SamplerCreateInfo samplerInfo(
		vk::SamplerCreateFlags(),
		vk::Filter::eLinear,
		vk::Filter::eLinear,
		vk::SamplerMipmapMode::eLinear,
		vk::SamplerAddressMode::eRepeat,
		vk::SamplerAddressMode::eRepeat,
		vk::SamplerAddressMode::eRepeat,
		0.f,
		VK_TRUE,
		16.f,
		VK_FALSE,
		vk::CompareOp::eAlways,
		0.f,
		0.f,
		vk::BorderColor::eIntOpaqueBlack,
		VK_FALSE
	);

	vk::PhysicalDeviceProperties properties = m_Device->GetPhysicalDevice().getProperties();
	if (properties.limits.maxSamplerAnisotropy > 0)
	{
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	}

	m_TextureSampler = m_Device->GetDevice().createSampler(samplerInfo);
}

void Renderer::DestroyTextureSampler()
{
	m_Device->GetDevice().destroySampler(m_TextureSampler);
}

void Renderer::CreateDescriptorSetLayout()
{
	vk::DescriptorSetLayoutBinding uboLayoutBinding(
		0,
		vk::DescriptorType::eUniformBuffer,
		1,
		vk::ShaderStageFlagBits::eVertex
	);
	vk::DescriptorSetLayoutBinding samplerLayoutBinding(
		1,
		vk::DescriptorType::eCombinedImageSampler,
		1,
		vk::ShaderStageFlagBits::eFragment
	);

	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	vk::DescriptorSetLayoutCreateInfo layoutInfo(
		vk::DescriptorSetLayoutCreateFlags(),
		static_cast<uint32_t>(bindings.size()),
		bindings.data()
	);

	m_DescriptorSetLayout = m_Device->GetDevice().createDescriptorSetLayout(layoutInfo);
}

void Renderer::DestroyDescriptorSetLayout()
{
	m_Device->GetDevice().destroyDescriptorSetLayout(m_DescriptorSetLayout);
}

void Renderer::CreateDescriptorPool()
{
	std::vector<vk::DescriptorPoolSize> poolSizes = {
		vk::DescriptorPoolSize(
			vk::DescriptorType::eUniformBuffer,
			static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
		),
		vk::DescriptorPoolSize(
			vk::DescriptorType::eCombinedImageSampler,
			static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
		)
	};

	vk::DescriptorPoolCreateInfo poolInfo(
		vk::DescriptorPoolCreateFlags(),
		static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
		static_cast<uint32_t>(poolSizes.size()),
		poolSizes.data()
	);

	m_DescriptorPool = m_Device->GetDevice().createDescriptorPool(poolInfo);
}

void Renderer::DestroyeDescriptorPool()
{
	m_Device->GetDevice().destroyDescriptorPool(m_DescriptorPool);
}

void Renderer::CreateDescriptorSets()
{
	std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_DescriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo( m_DescriptorPool, layouts.size(), layouts.data() );

	m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

	vk::Result result = m_Device->GetDevice().allocateDescriptorSets(&allocInfo, m_DescriptorSets.data());
	if (result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate descriptor sets");

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::DescriptorBufferInfo bufferInfo(
			m_UniformBuffers[i]->GetBuffer(),	// Buffer
			0,									// Offset
			sizeof(UniformBufferObject) 		// Range
		);

		vk::DescriptorImageInfo imageInfo(
			m_TextureSampler,						// Sampler
			m_TextureImageView,						// ImageView
			vk::ImageLayout::eShaderReadOnlyOptimal // ImageLayout
		);

		std::array<vk::WriteDescriptorSet, 2> descriptorWrites{
			vk::WriteDescriptorSet(
				m_DescriptorSets[i],					// Descriptor set
				0, 										// Binding
				0, 										// Array element
				1, vk::DescriptorType::eUniformBuffer, 	// Descriptor count / Descriptor type
				nullptr, 								// Descriptor image info
				&bufferInfo, 							// Descriptor buffer info
				nullptr 								// Descriptor texel buffer view
			),
			vk::WriteDescriptorSet(
				m_DescriptorSets[i],							// Descriptor set
				1, 												// Binding
				0, 												// Array element
				1, vk::DescriptorType::eCombinedImageSampler, 	// Descriptor count / Descriptor type
				&imageInfo, 									// Descriptor image info
				nullptr, 										// Descriptor buffer info
				nullptr 										// Descriptor texel buffer view
			)
		};

		m_Device->GetDevice().updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	}
}

void Renderer::CreateCommandBuffers()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::CommandBufferAllocateInfo allocInfo(m_Device->GetCommandPool(), vk::CommandBufferLevel::ePrimary, 1);

		vk::Result result = m_Device->GetDevice().allocateCommandBuffers(&allocInfo, &m_Frames[i].CommandBuffer);
		if (result != vk::Result::eSuccess)
			throw std::runtime_error("Failed to allocate command buffers");
	}
}

void Renderer::RecordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
	vk::CommandBufferBeginInfo beginInfo(
		vk::CommandBufferUsageFlagBits::eSimultaneousUse,
		nullptr
	);

	commandBuffer.begin(beginInfo);
	
	const vk::ClearValue clearValues[2]{
		{vk::ClearColorValue(std::array<float, 4>{.05f, 0.f, .05f, 1.f})},
		{vk::ClearDepthStencilValue(1.f, 0)}
	};

	vk::RenderPassBeginInfo renderPassInfo(
		m_RenderPass,
		m_SwapChainFramebuffers[imageIndex],
		vk::Rect2D( vk::Offset2D( 0, 0 ), m_SwapChainExtent ),
		2, clearValues
	);

	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	m_Pipeline->Bind(commandBuffer);

	vk::Buffer vertexBuffers[] = { m_VertexBuffer->GetBuffer() };
	vk::DeviceSize offsets[] = { 0 };
	commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

	if (indices.size() > 0)
		commandBuffer.bindIndexBuffer(m_IndexBuffer->GetBuffer(), 0, vk::IndexType::eUint16);

	vk::Viewport viewport(
		0.0f,
		0.0f,
		static_cast<float>(m_SwapChainExtent.width),
		static_cast<float>(m_SwapChainExtent.height),
		0.0f,
		1.0f
	);
	commandBuffer.setViewport(0, 1, &viewport);

	vk::Rect2D scissor(vk::Offset2D(0, 0), m_SwapChainExtent);
	commandBuffer.setScissor(0, 1, &scissor);

	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_Pipeline->GetLayout(), 0, 1, &m_DescriptorSets[m_CurrentFrame], 0, nullptr);

	if(indices.size() > 0)
		commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	else
		commandBuffer.draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);

	commandBuffer.endRenderPass();
	commandBuffer.end();
}

void Renderer::CreateSyncObjects()
{
	vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_Frames[i].RenderSemaphore = m_Device->GetDevice().createSemaphore(vk::SemaphoreCreateInfo());
		m_Frames[i].PresentSemaphore = m_Device->GetDevice().createSemaphore(vk::SemaphoreCreateInfo());
		m_Frames[i].RenderFence = m_Device->GetDevice().createFence(fenceInfo);

		if (!m_Frames[i].RenderSemaphore || !m_Frames[i].PresentSemaphore || !m_Frames[i].RenderFence)
			throw std::runtime_error("Failed to create sync objects");
	}
}

void Renderer::DestroySyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_Device->GetDevice().destroyFence(m_Frames[i].RenderFence);
		m_Device->GetDevice().destroySemaphore(m_Frames[i].RenderSemaphore);
		m_Device->GetDevice().destroySemaphore(m_Frames[i].PresentSemaphore);
	}
}

void Renderer::DrawFrame()
{
	if (m_FramebufferResized)
	{
		RecreateSwapChain();
		m_FramebufferResized = false;
	}

	while(vk::Result::eTimeout == m_Device->GetDevice().waitForFences(1, &m_Frames[m_CurrentFrame].RenderFence, VK_TRUE, UINT64_MAX));

	vk::ResultValue<uint32_t> currentBuffer = m_Device->GetDevice().acquireNextImageKHR(m_SwapChain, UINT64_MAX, m_Frames[m_CurrentFrame].PresentSemaphore, nullptr);

	if (currentBuffer.result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to acquire swap chain image");

	UpdateUniformbuffer(m_CurrentFrame);

	while (vk::Result::eTimeout == m_Device->GetDevice().resetFences(1, &m_Frames[m_CurrentFrame].RenderFence));
	m_Frames[m_CurrentFrame].CommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

	RecordCommandBuffer(m_Frames[m_CurrentFrame].CommandBuffer, currentBuffer.value);

	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

	vk::SubmitInfo submitInfo(
		1, &m_Frames[m_CurrentFrame].PresentSemaphore,
		waitStages,
		1, &m_Frames[m_CurrentFrame].CommandBuffer,
		1, &m_Frames[m_CurrentFrame].RenderSemaphore
	);

	vk::Result queueSubmitResult = m_Device->GetGraphicsQueue().submit(1, &submitInfo, m_Frames[m_CurrentFrame].RenderFence);
	if(queueSubmitResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to submit draw command buffer");

	vk::PresentInfoKHR presentInfo(
		1, &m_Frames[m_CurrentFrame].RenderSemaphore,
		1, &m_SwapChain,
		&currentBuffer.value
	);

	vk::Result queuePresentResult = m_Device->GetPresentQueue().presentKHR(presentInfo);
	if (queuePresentResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to present swap chain image");

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
