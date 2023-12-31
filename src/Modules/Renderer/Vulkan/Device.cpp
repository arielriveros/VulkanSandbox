#include <iostream>
#include <set>
#include "Device.h"

Device::Device(Window& window): m_Window(window) {}

Device::~Device() {}

void Device::CreateDevice()
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

void Device::DestroyDevice()
{
    m_Device.destroy();
}

void Device::CreateInstance()
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

void Device::DestroyInstance()
{
    m_Instance.destroy();
}

void Device::SelectPhysicalDevice()
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

bool Device::IsDeviceSuitable(vk::PhysicalDevice device)
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

QueueFamilyIndices Device::FindQueueFamilies(vk::PhysicalDevice device)
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

void Device::Initialize()
{
	CreateInstance();
    CreateValidationLayer();
    CreateSurface();
    SelectPhysicalDevice();
    CreateDevice();
	CreateCommandPool();
}

void Device::Terminate()
{
	DestroyCommandPool();
	DestroyDevice();
	DestroySurface();
	DestroyValidationLayer();
	DestroyInstance();
}

SwapChainSupportDetails Device::QuerySwapChainSupport(vk::PhysicalDevice device)
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

uint32_t Device::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
    vk::PhysicalDeviceMemoryProperties memProperties = m_PhysicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	throw std::runtime_error("Failed to find suitable memory type");
}

vk::Buffer Device::CreateBuffer(
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

	vk::Buffer buffer = m_Device.createBuffer(bufferInfo);

	vk::MemoryRequirements memRequirements = m_Device.getBufferMemoryRequirements(buffer);
	vk::MemoryAllocateInfo allocInfo(
		memRequirements.size,
		FindMemoryType(memRequirements.memoryTypeBits, properties)
	);

	vk::Result allocateResult = m_Device.allocateMemory(&allocInfo, nullptr, &bufferMemory);
	if (allocateResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate buffer memory");

	m_Device.bindBufferMemory(buffer, bufferMemory, 0);

	return buffer;
}

void Device::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();

	vk::BufferCopy copyRegion(0, 0, size);

	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
}

vk::Image Device::CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::DeviceMemory &imageMemory)
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

	vk::Image image = m_Device.createImage(imageInfo);

	vk::MemoryRequirements memRequirements = m_Device.getImageMemoryRequirements(image);
	vk::MemoryAllocateInfo allocInfo(
		memRequirements.size,
		FindMemoryType(memRequirements.memoryTypeBits, properties)
	);

	vk::Result allocateResult = m_Device.allocateMemory(&allocInfo, nullptr, &imageMemory);
	if (allocateResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate image memory");

	m_Device.bindImageMemory(image, imageMemory, 0);

	return image;
}

vk::ImageView Device::CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)
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

void Device::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
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

bool Device::CheckDeviceExtensionSupport(vk::PhysicalDevice device)
{
    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}

void Device::CreateSurface()
{
    m_Surface = m_Window.CreateSurface(m_Instance);
}

void Device::DestroySurface()
{
	m_Instance.destroySurfaceKHR(m_Surface);
}

void Device::CreateCommandPool()
{
	vk::CommandPoolCreateInfo poolInfo(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		m_QueueFamilies.GraphicsFamily.value()
	);

	vk::Result result = m_Device.createCommandPool(&poolInfo, nullptr, &m_CommandPool);
	if (result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create command pool");
}

void Device::DestroyCommandPool()
{
	m_Device.destroyCommandPool(m_CommandPool);
}

vk::CommandBuffer Device::BeginSingleTimeCommands()
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

void Device::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
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

void Device::CreateValidationLayer()
{
    m_ValidationLayer = new ValidationLayer();
	m_ValidationLayer->Init(m_Instance);
}

void Device::DestroyValidationLayer()
{
    m_ValidationLayer->Destroy();
	delete m_ValidationLayer;
}
