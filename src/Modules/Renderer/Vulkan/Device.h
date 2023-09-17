#pragma once
#include <vulkan/vulkan.hpp>
#include <optional>
#include "./ValidationLayer.h"
#include "../../../Window/Window.h"

struct QueueFamilyIndices
{
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;

	bool IsComplete()
	{
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	vk::SurfaceCapabilitiesKHR Capabilities;
	std::vector<vk::SurfaceFormatKHR> Formats;
	std::vector<vk::PresentModeKHR> PresentModes;
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class Device
{
public:
    Device(Window* window);
    ~Device();

    Device(const Device &) = delete;
    void operator=(const Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &&) = delete;

    vk::Instance GetInstance() const { return m_Instance; }
    vk::PhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    vk::Device GetDevice() const { return m_Device; }
    vk::Queue GetGraphicsQueue() const { return m_GraphicsQueue; }
    vk::Queue GetPresentQueue() const { return m_PresentQueue; }
    vk::SurfaceKHR GetSurface() const { return m_Surface; }
    QueueFamilyIndices GetQueueFamilies() const { return m_QueueFamilies; }

    void Initialize();
    void Terminate();
    void WaitIdle() { m_Device.waitIdle(); }
    SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device);

private:
    void CreateDevice();
    void DestroyDevice();
    void CreateInstance();
    void DestroyInstance();
    void SelectPhysicalDevice();
    bool IsDeviceSuitable(vk::PhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);
    bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);
    void CreateSurface();
    void DestroySurface();

    void CreateValidationLayer();
    void DestroyValidationLayer();

    Window* m_Window;
    vk::Instance m_Instance;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::Device m_Device;
    vk::Queue m_GraphicsQueue;
    vk::Queue m_PresentQueue;
    vk::SurfaceKHR m_Surface;
    QueueFamilyIndices m_QueueFamilies;
    SwapChainSupportDetails m_SwapChainSupport;

    ValidationLayer* m_ValidationLayer;
};