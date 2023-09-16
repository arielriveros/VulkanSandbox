#include <iostream>
#include <set>
#include <array>
#include <chrono>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Renderer.h"


Renderer::Renderer(Window* window) : m_Window{ window }
{
	std::cout << "Renderer Constructor" << std::endl;
	Resize(m_Window->Width, m_Window->Height);
}

Renderer::~Renderer()
{
	std::cout << "Renderer Destructor" << std::endl;
}

void Renderer::Initialize()
{
	CreateInstance();
	CreateValidationLayer();
	CreateSurface();
	SelectPhysicalDevice();
	CreateDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateDescriptorSetLayout();
	CreateGraphicsPipeline();
	CreateDepthResources();
	CreateFramebuffers();
	CreateCommandPool();
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

void Renderer::Update()
{
	DrawFrame();
}

void Renderer::WaitIdle()
{
	m_Device.waitIdle();
}

void Renderer::Terminate()
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
	DestroyGraphicsPipeline();
	DestroyRenderPass();
	DestroyUniformBuffers();
	DestroySyncObjects();
	DestroyCommandPool();
	DestroyDevice();
	DestroySurface();
	DestroyValidationLayer();
	DestroyInstance();
}

void Renderer::Resize(uint32_t width, uint32_t height)
{
	if (m_Width == width && m_Height == height) return;
	m_Width = width;
	m_Height = height;
	m_FramebufferResized = true;
}

void Renderer::CreateInstance()
{
	try
	{
		uint32_t version = 0;
		vkEnumerateInstanceVersion(&version);
		std::cout << "Vulkan Version: " << VK_API_VERSION_MAJOR(version) << '.' << VK_API_VERSION_MINOR(version) << '.' << VK_API_VERSION_PATCH(version) << std::endl;

		vk::ApplicationInfo appInfo("Vulkan Sandbox", 1, "No Engine", 1, VK_API_VERSION_1_0);
		auto extensions = m_ValidationLayer->GetRequiredExtensions();
		vk::InstanceCreateInfo createInfo( {}, &appInfo, 0, nullptr, static_cast<uint32_t>(extensions.size()), extensions.data() );

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

		if (ENABLE_VALIDATION_LAYERS)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			m_ValidationLayer->PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}

		if (ENABLE_VALIDATION_LAYERS && !m_ValidationLayer->CheckValidationLayerSupport())
			throw std::runtime_error("Validation layers requested, but not available");

		vk::Instance instance = vk::createInstance( createInfo );
		m_Instance = instance;
	}

	catch ( vk::SystemError & err )
	{
		std::cout << "vk::SystemError: " << err.what() << std::endl;
		exit( -1 );
	}

}

void Renderer::DestroyInstance()
{
	m_Instance.destroy();
}

void Renderer::SelectPhysicalDevice()
{
	try
	{
		uint32_t deviceCount = 0;
		std::vector<vk::PhysicalDevice> devices = m_Instance.enumeratePhysicalDevices();

		if (devices.size() == 0)
			throw std::runtime_error("Failed to find GPUs with Vulkan support");

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				m_PhysicalDevice = device;
				break;
			}
		}
	}

	catch ( vk::SystemError & err )
	{
		std::cout << "vk::SystemError: " << err.what() << std::endl;
		exit( -1 );
	}
}

QueueFamilyIndices Renderer::FindQueueFamilies(vk::PhysicalDevice device)
{
	QueueFamilyIndices indices;
	
	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		VkBool32 presentSupport = false;
		vk::Result result = device.getSurfaceSupportKHR(i, m_Surface, &presentSupport);

		if (result != vk::Result::eSuccess)
			throw std::runtime_error("Failed to get surface support");

		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			indices.GraphicsFamily = i;
		
		if (presentSupport)
			indices.PresentFamily = i;

		if (indices.IsComplete())
			break;

		i++;
	}

	return indices;
}

bool Renderer::IsDeviceSuitable(vk::PhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
	}

	vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();

	return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

void Renderer::CreateDevice()
{
	try
	{
		m_QueueFamilies = FindQueueFamilies(m_PhysicalDevice);
		
		float queuePriority = 1.0f;

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			m_QueueFamilies.GraphicsFamily.value(),
			m_QueueFamilies.PresentFamily.value()
		};

		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			vk::DeviceQueueCreateInfo queueCreateInfo( vk::DeviceQueueCreateFlags(), static_cast<uint32_t>( queueFamily ), 1, &queuePriority );
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures deviceFeatures = m_PhysicalDevice.getFeatures();

		vk::DeviceCreateInfo createInfo(
			vk::DeviceCreateFlags(),
			static_cast<uint32_t>( queueCreateInfos.size() ),
			queueCreateInfos.data(),
			0,
			nullptr,
			static_cast<uint32_t>( deviceExtensions.size() ),
			deviceExtensions.data(),
			&deviceFeatures
		);

		m_Device = m_PhysicalDevice.createDevice( createInfo );

		m_GraphicsQueue = m_Device.getQueue(m_QueueFamilies.GraphicsFamily.value(), 0);
		m_PresentQueue = m_Device.getQueue(m_QueueFamilies.PresentFamily.value(), 0);
	}
	catch ( vk::SystemError & err )
	{
		std::cout << "vk::SystemError: " << err.what() << std::endl;
		exit( -1 );
	}
}

void Renderer::DestroyDevice()
{
	m_Device.destroy();
}

void Renderer::CreateSurface()
{
	if (glfwCreateWindowSurface(m_Instance, m_Window->GetWindow(), nullptr, &m_Surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface");
}

void Renderer::DestroySurface()
{
	m_Instance.destroySurfaceKHR(m_Surface);
}

bool Renderer::CheckDeviceExtensionSupport(vk::PhysicalDevice device)
{
	std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}

SwapChainSupportDetails Renderer::QuerySwapChainSupport(vk::PhysicalDevice device)
{
	SwapChainSupportDetails details;
	
	details.Capabilities = device.getSurfaceCapabilitiesKHR(m_Surface);
	details.Formats = device.getSurfaceFormatsKHR( m_Surface );

	if (details.Formats.empty())
		throw std::runtime_error("Failed to get surface formats");

	details.PresentModes = device.getSurfacePresentModesKHR( m_Surface );
	if (details.PresentModes.empty())
		throw std::runtime_error("Failed to get surface present modes");

	return details;
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
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice);

	vk::SurfaceFormatKHR surfaceFormat = SelectSwapSurfaceFormat(swapChainSupport.Formats);
	vk::PresentModeKHR presentMode = SelectSwapPresentMode(swapChainSupport.PresentModes);
	vk::Extent2D extent = SelectSwapExtent(swapChainSupport.Capabilities);

	uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;

	if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
		imageCount = swapChainSupport.Capabilities.maxImageCount;

	vk::SwapchainCreateInfoKHR createInfo(
		vk::SwapchainCreateFlagsKHR(),
		m_Surface,
		imageCount,
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment
	);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo( vk::SwapchainCreateFlagsKHR(),
		m_Surface,
		imageCount,
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive
	);

	uint32_t queueFamilyIndices[2] = { m_QueueFamilies.GraphicsFamily.value(), m_QueueFamilies.PresentFamily.value() };

	if (m_QueueFamilies.GraphicsFamily != m_QueueFamilies.PresentFamily)
	{
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}

	m_SwapChain = m_Device.createSwapchainKHR(createInfo);

	m_SwapChainImages = m_Device.getSwapchainImagesKHR(m_SwapChain);
	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

void Renderer::DestroySwapChain()
{
	m_Device.destroySwapchainKHR(m_SwapChain);
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
		m_Device.destroyImageView(imageView);
}

void Renderer::RecreateSwapChain()
{
	while (m_Width == 0 || m_Height == 0)
		glfwWaitEvents();

	WaitIdle();

	DestroyFramebuffers();
	DestroyDepthResources();
	DestroyImageViews();
	DestroySwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateDepthResources();
	CreateFramebuffers();
}

void Renderer::CreateGraphicsPipeline()
{
	m_Pipeline = new Pipeline(m_Device, m_RenderPass, m_DescriptorSetLayout);
	m_Pipeline->Create("resources/shaders/default.vert.spv", "resources/shaders/default.frag.spv", Vertex::GetDescriptions());
}

void Renderer::DestroyGraphicsPipeline()
{
	delete m_Pipeline;
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

	m_RenderPass = m_Device.createRenderPass(renderPassInfo);
}

void Renderer::DestroyRenderPass()
{
	m_Device.destroyRenderPass(m_RenderPass);
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

		m_SwapChainFramebuffers[i] = m_Device.createFramebuffer(framebufferInfo);
	}
}

void Renderer::DestroyFramebuffers()
{
	for (auto framebuffer : m_SwapChainFramebuffers)
		m_Device.destroyFramebuffer(framebuffer);
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
	m_Device.destroyImageView(m_DepthImageView);
	m_Device.destroyImage(m_DepthImage);
	m_Device.freeMemory(m_DepthImageMemory);
}

void Renderer::CreateVertexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	
	vk::DeviceMemory stagingBufferMemory;

	vk::Buffer stagingBuffer = CreateBuffer(
		bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBufferMemory
	);

	void* data = m_Device.mapMemory(stagingBufferMemory, 0, bufferSize);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	m_Device.unmapMemory(stagingBufferMemory);

	m_VertexBuffer = CreateBuffer(
		bufferSize,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		m_VertexBufferMemory
	);

	CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

	m_Device.destroyBuffer(stagingBuffer);
	m_Device.freeMemory(stagingBufferMemory);
}

void Renderer::DestroyVertexBuffer()
{
	m_Device.destroyBuffer(m_VertexBuffer);
	m_Device.freeMemory(m_VertexBufferMemory);
}

void Renderer::CreateIndexBuffer()
{
	if (indices.size() <= 0)
		return;

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	vk::DeviceMemory stagingBufferMemory;
	vk::Buffer stagingBuffer = CreateBuffer(
		bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBufferMemory
	);

	void* data;
	vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_Device, stagingBufferMemory);

	m_IndexBuffer = CreateBuffer(
		bufferSize,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		m_IndexBufferMemory
	);

	CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

	m_Device.destroyBuffer(stagingBuffer);
	m_Device.freeMemory(stagingBufferMemory);
}

void Renderer::DestroyIndexBuffer()
{
	if (indices.size() <= 0)
		return;

	vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
	vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);
}

void Renderer::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	m_UniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	m_UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_UniformBuffers[i] = CreateBuffer(
			bufferSize,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			m_UniformBuffersMemory[i]
		);
		m_UniformBuffersMapped[i] = m_Device.mapMemory(m_UniformBuffersMemory[i], 0, bufferSize, vk::MemoryMapFlags());
	}
}

void Renderer::DestroyUniformBuffers()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyBuffer(m_Device, m_UniformBuffers[i], nullptr);
		vkFreeMemory(m_Device, m_UniformBuffersMemory[i], nullptr);
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

	memcpy(m_UniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

uint32_t Renderer::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)	
{
	vk::PhysicalDeviceMemoryProperties memProperties = m_PhysicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	throw std::runtime_error("Failed to find suitable memory type");
}

vk::Buffer Renderer::CreateBuffer(
	vk::DeviceSize size,
	vk::BufferUsageFlags usage,
	vk::MemoryPropertyFlags properties,
	vk::DeviceMemory& bufferMemory)
{
	vk::BufferCreateInfo bufferInfo(
		vk::BufferCreateFlags(),
		size,
		usage,
		vk::SharingMode::eExclusive
	);

	vk::Buffer vertexBuffer = m_Device.createBuffer(bufferInfo);

	vk::MemoryRequirements memRequirements = m_Device.getBufferMemoryRequirements(vertexBuffer);
	vk::MemoryAllocateInfo allocInfo(
		memRequirements.size,
		FindMemoryType(memRequirements.memoryTypeBits, properties)
	);

	vk::Result allocateResult = m_Device.allocateMemory(&allocInfo, nullptr, &bufferMemory);
	if (allocateResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate buffer memory");

	m_Device.bindBufferMemory(vertexBuffer, bufferMemory, 0);

	return vertexBuffer;
}

void Renderer::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

	vk::BufferCopy copyRegion(0, 0, size);

	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
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
	vk::Buffer stagingBuffer = CreateBuffer(
		imageSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		stagingBufferMemory
	);

	void* data = m_Device.mapMemory(stagingBufferMemory, 0, imageSize, vk::MemoryMapFlags());
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	m_Device.unmapMemory(stagingBufferMemory);

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

	m_Device.destroyBuffer(stagingBuffer);
	m_Device.freeMemory(stagingBufferMemory);
}

void Renderer::DestroyTextureImage()
{
	m_Device.destroyImage(m_TextureImage);
	m_Device.freeMemory(m_TextureImageMemory);
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

	image = m_Device.createImage(imageInfo);

	vk::MemoryRequirements memRequirements = m_Device.getImageMemoryRequirements(image);
	vk::MemoryAllocateInfo allocInfo(
		memRequirements.size,
		FindMemoryType(memRequirements.memoryTypeBits, properties)
	);

	vk::Result allocateResult = m_Device.allocateMemory(&allocInfo, nullptr, &imageMemory);
	if (allocateResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate image memory");

	m_Device.bindImageMemory(image, imageMemory, 0);
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

	return m_Device.createImageView(viewInfo);
}

void Renderer::CreateTextureImageViews()
{
	m_TextureImageView = CreateImageView(m_TextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
}

void Renderer::DestroyTextureImageViews()
{
	m_Device.destroyImageView(m_TextureImageView);
}

void Renderer::TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

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

	EndSingleTimeCommands(commandBuffer);
}

void Renderer::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

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

	EndSingleTimeCommands(commandBuffer);
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

	vk::PhysicalDeviceProperties properties = m_PhysicalDevice.getProperties();
	if (properties.limits.maxSamplerAnisotropy > 0)
	{
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = m_PhysicalDevice.getProperties().limits.maxSamplerAnisotropy;
	}

	m_TextureSampler = m_Device.createSampler(samplerInfo);
}

void Renderer::DestroyTextureSampler()
{
	m_Device.destroySampler(m_TextureSampler);
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

	m_DescriptorSetLayout = m_Device.createDescriptorSetLayout(layoutInfo);
}

void Renderer::DestroyDescriptorSetLayout()
{
	m_Device.destroyDescriptorSetLayout(m_DescriptorSetLayout);
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

	m_DescriptorPool = m_Device.createDescriptorPool(poolInfo);
}

void Renderer::DestroyeDescriptorPool()
{
	m_Device.destroyDescriptorPool(m_DescriptorPool);
}

void Renderer::CreateDescriptorSets()
{
	std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_DescriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo( m_DescriptorPool, layouts.size(), layouts.data() );

	m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

	vk::Result result = m_Device.allocateDescriptorSets(&allocInfo, m_DescriptorSets.data());
	if (result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate descriptor sets");

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::DescriptorBufferInfo bufferInfo(
			m_UniformBuffers[i],		// Buffer
			0,							// Offset
			sizeof(UniformBufferObject) // Range
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

		m_Device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	}
}

void Renderer::CreateCommandPool()
{
	vk::CommandPoolCreateInfo poolInfo(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		m_QueueFamilies.GraphicsFamily.value()
	);

	vk::Result result = m_Device.createCommandPool(&poolInfo, nullptr, &m_CommandPool);
	if (result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create command pool");
}

void Renderer::DestroyCommandPool()
{
	m_Device.destroyCommandPool(m_CommandPool);
}

void Renderer::CreateCommandBuffers()
{
	m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	vk::CommandBufferAllocateInfo allocInfo(
		m_CommandPool,
		vk::CommandBufferLevel::ePrimary,
		static_cast<uint32_t>(m_CommandBuffers.size())
	);

	vk::Result result = m_Device.allocateCommandBuffers(&allocInfo, m_CommandBuffers.data());

	if (result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate command buffers");
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

	vk::Buffer vertexBuffers[] = { m_VertexBuffer };
	vk::DeviceSize offsets[] = { 0 };
	commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

	if (indices.size() > 0)
		commandBuffer.bindIndexBuffer(m_IndexBuffer, 0, vk::IndexType::eUint16);

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

vk::CommandBuffer Renderer::BeginSingleTimeCommands()
{
    vk::CommandBufferAllocateInfo allocInfo(
		m_CommandPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);

	vk::CommandBuffer commandBuffer = m_Device.allocateCommandBuffers(allocInfo).front();

	vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void Renderer::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo(
		0,
		nullptr,
		nullptr,
		1,
		&commandBuffer,
		0,
		nullptr
	);

	vk::Result submitResult = m_GraphicsQueue.submit(1, &submitInfo, nullptr);
	if (submitResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to submit single time command buffer");

	m_GraphicsQueue.waitIdle();

	m_Device.freeCommandBuffers(m_CommandPool, 1, &commandBuffer);
}

void Renderer::CreateSyncObjects()
{
	m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_ImageAvailableSemaphores[i] = m_Device.createSemaphore(vk::SemaphoreCreateInfo());
		m_RenderFinishedSemaphores[i] = m_Device.createSemaphore(vk::SemaphoreCreateInfo());
		m_InFlightFences[i] = m_Device.createFence(fenceInfo);

		if (!m_ImageAvailableSemaphores[i] || !m_RenderFinishedSemaphores[i] || !m_InFlightFences[i])
			throw std::runtime_error("Failed to create sync objects");
	}
}

void Renderer::DestroySyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_Device.destroyFence(m_InFlightFences[i]);
		m_Device.destroySemaphore(m_RenderFinishedSemaphores[i]);
		m_Device.destroySemaphore(m_ImageAvailableSemaphores[i]);
	}
}

void Renderer::DrawFrame()
{
    try
    {
        if (m_FramebufferResized)
        {
            RecreateSwapChain();
            m_FramebufferResized = false;
        }

        while(vk::Result::eTimeout == m_Device.waitForFences(1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX));

        vk::ResultValue<uint32_t> currentBuffer = m_Device.acquireNextImageKHR(m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], nullptr);

        if (currentBuffer.result != vk::Result::eSuccess)
			throw std::runtime_error("Failed to acquire swap chain image");

        UpdateUniformbuffer(m_CurrentFrame);

        while (vk::Result::eTimeout == m_Device.resetFences(1, &m_InFlightFences[m_CurrentFrame]));
        m_CommandBuffers[m_CurrentFrame].reset(vk::CommandBufferResetFlagBits::eReleaseResources);

        RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], currentBuffer.value);

        vk::Semaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        vk::Semaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };

        vk::SubmitInfo submitInfo(
            1, waitSemaphores,
            waitStages,
            1, &m_CommandBuffers[m_CurrentFrame],
            1, signalSemaphores
        );

        vk::Result queueSubmitResult = m_GraphicsQueue.submit(1, &submitInfo, m_InFlightFences[m_CurrentFrame]);
        if(queueSubmitResult != vk::Result::eSuccess)
			throw std::runtime_error("Failed to submit draw command buffer");

        vk::PresentInfoKHR presentInfo(
            1, signalSemaphores,
            1, &m_SwapChain,
            &currentBuffer.value
        );

        vk::Result queuePresentResult = m_PresentQueue.presentKHR(presentInfo);
        if (queuePresentResult != vk::Result::eSuccess)
			throw std::runtime_error("Failed to present swap chain image");

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    catch (vk::SystemError& err)
    {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        exit(-1);
    }
}

void Renderer::CreateValidationLayer()
{
	m_ValidationLayer = new ValidationLayer();
	m_ValidationLayer->Init(m_Instance);
}

void Renderer::DestroyValidationLayer()
{
	m_ValidationLayer->Destroy();
	delete m_ValidationLayer;
}
