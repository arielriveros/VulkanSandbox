#include "SwapChain.h"

SwapChain::SwapChain(Device& device, Window& window)
	: m_Device(device), m_Window(window)
{
}

SwapChain::~SwapChain()
{
}

void SwapChain::Initialize()
{
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateDepthResources();
	CreateFramebuffers();
}

void SwapChain::Terminate()
{
	DestroySwapChain();
	DestroyFramebuffers();
	DestroyImageViews();
	DestroyDepthResources();
	DestroyRenderPass();
}

void SwapChain::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = m_Device.QuerySwapChainSupport(m_Device.GetPhysicalDevice());

	vk::SurfaceFormatKHR surfaceFormat = SelectSwapSurfaceFormat(swapChainSupport.Formats);
	vk::PresentModeKHR presentMode = SelectSwapPresentMode(swapChainSupport.PresentModes);
	vk::Extent2D extent = SelectSwapExtent(swapChainSupport.Capabilities);

	uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;

	if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
		imageCount = swapChainSupport.Capabilities.maxImageCount;

	vk::SwapchainCreateInfoKHR createInfo(
		vk::SwapchainCreateFlagsKHR(),
		m_Device.GetSurface(),
		imageCount,
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment
	);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo( vk::SwapchainCreateFlagsKHR(),
		m_Device.GetSurface(),
		imageCount,
		surfaceFormat.format,
		surfaceFormat.colorSpace,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive
	);

	uint32_t queueFamilyIndices[2] = {
		m_Device.GetQueueFamilies().GraphicsFamily.value(),
		m_Device.GetQueueFamilies().PresentFamily.value()
	};

	if (m_Device.GetQueueFamilies().GraphicsFamily != m_Device.GetQueueFamilies().PresentFamily)
	{
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}

	m_SwapChain = m_Device.GetDevice().createSwapchainKHR(createInfo);

	m_Images = m_Device.GetDevice().getSwapchainImagesKHR(m_SwapChain);
	m_ImageFormat = surfaceFormat.format;
	m_Extent = extent;
}

void SwapChain::DestroySwapChain()
{
	m_Device.GetDevice().destroySwapchainKHR(m_SwapChain);
}

void SwapChain::CreateImageViews()
{
	m_ImageViews.resize(m_Images.size());

	for (size_t i = 0; i < m_Images.size(); i++)
		m_ImageViews[i] = m_Device.CreateImageView(m_Images[i], m_ImageFormat, vk::ImageAspectFlagBits::eColor);
}

void SwapChain::DestroyImageViews()
{
	for (auto imageView : m_ImageViews)
		m_Device.GetDevice().destroyImageView(imageView);
}

void SwapChain::CreateDepthResources()
{
	vk::Format depthFormat = vk::Format::eD32SfloatS8Uint;

	m_DepthImage = m_Device.CreateImage(
		m_Extent.width,
		m_Extent.height,
		depthFormat,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		m_DepthImageMemory
	);

	m_DepthImageView = m_Device.CreateImageView(m_DepthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

void SwapChain::DestroyDepthResources()
{
	m_Device.GetDevice().destroyImageView(m_DepthImageView);
	m_Device.GetDevice().destroyImage(m_DepthImage);
	m_Device.GetDevice().freeMemory(m_DepthImageMemory);
}

void SwapChain::Recreate()
{
	DestroyFramebuffers();
	DestroyDepthResources();
	DestroyImageViews();
	DestroySwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateDepthResources();
	CreateFramebuffers();
}

void SwapChain::CreateFramebuffers()
{
	m_Framebuffers.resize(m_ImageViews.size());
	for (size_t i = 0; i < m_ImageViews.size(); i++)
	{
		vk::ImageView attachments[] = {
			m_ImageViews[i],
			m_DepthImageView
		};

		vk::FramebufferCreateInfo framebufferInfo(
			vk::FramebufferCreateFlags(),
			m_RenderPass,
			2, attachments,
			m_Extent.width,
			m_Extent.height,
			1
		);

		m_Framebuffers[i] = m_Device.GetDevice().createFramebuffer(framebufferInfo);
	}
}

void SwapChain::DestroyFramebuffers()
{
	for (auto framebuffer : m_Framebuffers)
		m_Device.GetDevice().destroyFramebuffer(framebuffer);
}

void SwapChain::CreateRenderPass()
{
	vk::AttachmentDescription colorAttachment(
		vk::AttachmentDescriptionFlags(),
		m_ImageFormat,
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

	m_RenderPass = m_Device.GetDevice().createRenderPass(renderPassInfo);
}

void SwapChain::DestroyRenderPass()
{
	m_Device.GetDevice().destroyRenderPass(m_RenderPass);
}

vk::SurfaceFormatKHR SwapChain::SelectSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
{
    for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return availableFormat;

	return availableFormats[0];
}

vk::PresentModeKHR SwapChain::SelectSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
		if (availablePresentMode == vk::PresentModeKHR::eMailbox)
			m_PresentMode = availablePresentMode;

	m_PresentMode = vk::PresentModeKHR::eFifo;
	return m_PresentMode;
}

vk::Extent2D SwapChain::SelectSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;

	else {
		int width, height;
		glfwGetFramebufferSize(m_Window.GetWindow(), &width, &height);

		vk::Extent2D actualExtent(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}
