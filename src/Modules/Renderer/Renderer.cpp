#include <iostream>
#include <array>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Renderer.h"


Renderer::Renderer(Window& window, Camera& camera, std::vector<Model*> models)
	: m_Window{ window }, m_Camera{ camera }, m_Models{ models }
{
	std::cout << "Renderer Constructor" << std::endl;
	Resize(m_Window.Width, m_Window.Height);
	m_Camera.UpdateRotation();
}

Renderer::~Renderer()
{
	std::cout << "Renderer Destructor" << std::endl;
	
	delete m_SwapChain;
	delete m_Pipeline;
	delete m_Device;
}

void Renderer::Initialize()
{
	try
	{
		m_Device = new Device(m_Window);
		m_Device->Initialize();
		m_SwapChain = new SwapChain(*m_Device, m_Window);
		m_SwapChain->Initialize();
		CreateDescriptorSetLayout();
		m_Pipeline = new Pipeline(m_Device->GetDevice(), m_SwapChain->GetRenderPass());
		m_Pipeline->Create(
			"resources/shaders/default.vert.spv",
			"resources/shaders/default.frag.spv",
			Vertex::GetDescriptions(),
			m_DescriptorSetLayout,
			sizeof(PushConstantData));
		CreateTextureImage();
		CreateTextureImageViews();
		CreateTextureSampler();
		SetupMeshes();
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
		DestroyMeshes();
		DestroyTextureImageViews();
		DestroyTextureImage();
		DestroyTextureSampler();
		DestroyeDescriptorPool();
		DestroyDescriptorSetLayout();
		m_Pipeline->Terminate();
		DestroyUniformBuffers();
		DestroySyncObjects();
		m_SwapChain->Terminate();
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
	m_Camera.Resize(m_Width, m_Height);
}

void Renderer::SetupMeshes()
{
	for (Model* model : m_Models)
	{
		MeshData meshData = model->GetMeshData();
		Mesh* mesh = new Mesh(*m_Device);
		mesh->Create(meshData.Vertices, meshData.Indices);
		m_Meshes.insert({ model->GetName(), mesh });
	}
}

void Renderer::DestroyMeshes()
{
	for (std::pair<std::string, Mesh*> mesh : m_Meshes)
	{
		mesh.second->Destroy();
		delete mesh.second;
	}
}

void Renderer::RecreateSwapChain()
{
	while (m_Width == 0 || m_Height == 0)
		glfwWaitEvents();

	m_Device->WaitIdle();
	m_SwapChain->Recreate();
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

void Renderer::UpdateUniformBuffer(uint32_t currentImage)
{
	UniformBufferObject ubo{};
	ubo.ViewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();

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
	
	m_TextureImage = m_Device->CreateImage(
		textureWidth,
		textureHeight,
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
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

void Renderer::CreateTextureImageViews()
{
	m_TextureImageView = m_Device->CreateImageView(m_TextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
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
		m_SwapChain->GetRenderPass(),
		m_SwapChain->GetFramebuffer(imageIndex),
		vk::Rect2D( vk::Offset2D( 0, 0 ), m_SwapChain->GetExtent() ),
		2, clearValues
	);

	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	vk::Viewport viewport(
		0.0f,
		0.0f,
		static_cast<float>(m_SwapChain->GetExtent().width),
		static_cast<float>(m_SwapChain->GetExtent().height),
		0.0f,
		1.0f
	);
	commandBuffer.setViewport(0, 1, &viewport);

	vk::Rect2D scissor(vk::Offset2D(0, 0), m_SwapChain->GetExtent());
	commandBuffer.setScissor(0, 1, &scissor);

	m_Pipeline->Bind(commandBuffer);
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_Pipeline->GetLayout(), 0, 1, &m_DescriptorSets[m_CurrentFrame], 0, nullptr);

	for (uint32_t i = 0; i < m_Models.size(); i++)
	{
		Mesh* mesh = m_Meshes[m_Models[i]->GetName()];
		
		PushConstantData pushConstantData{};
		pushConstantData.Model = m_Models[i]->GetModelMatrix();
		commandBuffer.pushConstants(m_Pipeline->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstantData), &pushConstantData);

		mesh->Bind(commandBuffer);
		if(mesh->IsIndexed())
			commandBuffer.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);

		else
			commandBuffer.draw(mesh->GetVertexSize(), 1, 0, 0);
	}

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

	vk::ResultValue<uint32_t> currentBuffer = m_Device->GetDevice().acquireNextImageKHR(m_SwapChain->GetSwapChain(),
																						UINT64_MAX,
																						m_Frames[m_CurrentFrame].PresentSemaphore,
																						nullptr);

	if (currentBuffer.result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to acquire swap chain image");

	UpdateUniformBuffer(m_CurrentFrame);

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
		1, &m_SwapChain->GetSwapChain(),
		&currentBuffer.value
	);

	vk::Result queuePresentResult = m_Device->GetPresentQueue().presentKHR(presentInfo);
	if (queuePresentResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to present swap chain image");

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
