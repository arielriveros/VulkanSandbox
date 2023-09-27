#pragma once
#include <vulkan/vulkan.hpp>
#include "Device.h"
#include "../../../Window/Window.h"

class SwapChain
{
public: 
	SwapChain(Device& device, Window& window);
	~SwapChain();

	SwapChain(const SwapChain &) = delete;
	void operator=(const SwapChain &) = delete;
	SwapChain(SwapChain &&) = delete;
	SwapChain &operator=(SwapChain &&) = delete;

	void Initialize();
	void Terminate();
	void Recreate();

	vk::SwapchainKHR GetSwapChain() const { return m_SwapChain; }
	vk::Format GetImageFormat() const { return m_ImageFormat; }
	vk::Extent2D GetExtent() const { return m_Extent; }
	vk::RenderPass GetRenderPass() const { return m_RenderPass; }
	vk::Framebuffer GetFramebuffer(uint32_t index) const { return m_Framebuffers[index]; }
	vk::PresentModeKHR GetPresentMode() const { return m_PresentMode; }

private:
	void CreateSwapChain();
	void DestroySwapChain();
	void CreateImageViews();
	void DestroyImageViews();
	void CreateDepthResources();
	void DestroyDepthResources();
	
	void CreateFramebuffers();
	void DestroyFramebuffers();
	void CreateRenderPass();
	void DestroyRenderPass();

	vk::SurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR SelectSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D SelectSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

	Device& m_Device;
	Window& m_Window;
	vk::RenderPass m_RenderPass;

	vk::SwapchainKHR m_SwapChain;
	std::vector<vk::Image> m_Images;
	std::vector<vk::ImageView> m_ImageViews;
	std::vector<vk::Framebuffer> m_Framebuffers;
	vk::Format m_ImageFormat;
	vk::Extent2D m_Extent = {0, 0};

	vk::PresentModeKHR m_PresentMode;

	vk::Image m_DepthImage;
	vk::DeviceMemory m_DepthImageMemory;
	vk::ImageView m_DepthImageView;
};