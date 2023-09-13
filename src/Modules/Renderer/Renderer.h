#include <vulkan/vulkan.hpp>
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "ValidationLayer/ValidationLayer.h"
#include "../ModuleInterface.h"
#include "../../Window/Window.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

struct QueueFamilyIndices
{
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;

	bool IsComplete()
	{
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct SwapChainSupportDetails
{
	vk::SurfaceCapabilitiesKHR Capabilities;
	std::vector<vk::SurfaceFormatKHR> Formats;
	std::vector<vk::PresentModeKHR> PresentModes;
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Color;

	static vk::VertexInputBindingDescription GetBindingDescription()
	{
		return vk::VertexInputBindingDescription(
			/*binding*/ 	0,
			/*stride*/ 		sizeof(Vertex),
			/*inputRate*/ 	vk::VertexInputRate::eVertex
		);
	}

	static std::array<vk::VertexInputAttributeDescription, 2> GetAttributeDescriptions()
	{
		std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;
};

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};

class Renderer: public IModule
{
public:
	Renderer(Window* window);
	~Renderer();

	void Initialize();
	void Update();
	void WaitIdle();
	void Terminate();
	void Resize(uint32_t width, uint32_t height);

private:
	void CreateInstance();
	void DestroyInstance();
	void SelectPhysicalDevice();
	QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);
	bool IsDeviceSuitable(vk::PhysicalDevice device);
	void CreateDevice();
	void DestroyDevice();
	void CreateSurface();
	void DestroySurface();
	
	bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device);
	vk::SurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR SelectSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D SelectSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
	void CreateSwapChain();
	void DestroySwapChain();
	void CreateImageViews();
	void DestroyImageViews();
	void RecreateSwapChain();

	void CreateGraphicsPipeline();
	void DestroyGraphicsPipeline();
	void DestroyPipelineLayout();
	std::vector<char> ReadFile(const std::string& filename); // Move to specialized class later
	vk::ShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreateRenderPass();
	void DestroyRenderPass();

	void CreateFramebuffers();
	void DestroyFramebuffers();

	void CreateVertexBuffer();
	void DestroyVertexBuffer();
	void CreateIndexBuffer();
	void DestroyIndexBuffer();
	void CreateUniformBuffers();
	void DestroyUniformBuffers();
	void UpdateUniformbuffer(uint32_t currentImage);

	uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

	vk::Buffer CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::DeviceMemory& bufferMemory);
	void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

	void CreateDescriptorSetLayout();
	void DestroyDescriptorSetLayout();
	void CreateDescriptorPool();
	void DestroyeDescriptorPool();
	void CreateDescriptorSets();

	void CreateCommandPool();
	void DestroyCommandPool();
	void CreateCommandBuffers();
	void RecordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

	void CreateSyncObjects();
	void DestroySyncObjects();

	void DrawFrame();

	void CreateValidationLayer();
	void DestroyValidationLayer();
private:
	uint32_t m_Width, m_Height;
	bool m_FramebufferResized = false;

	Window* m_Window;

	vk::Instance m_Instance;
	vk::PhysicalDevice m_PhysicalDevice;
	vk::Device m_Device;
	vk::Queue m_GraphicsQueue;
	vk::Queue m_PresentQueue;
	QueueFamilyIndices m_QueueFamilies;

	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
	vk::SwapchainKHR m_SwapChain;
	vk::RenderPass m_RenderPass;
	vk::Pipeline m_GraphicsPipeline;
	vk::PipelineLayout m_PipelineLayout;
	vk::DescriptorSetLayout m_DescriptorSetLayout;

	std::vector<vk::Image> m_SwapChainImages;
	vk::Format m_SwapChainImageFormat;
	vk::Extent2D m_SwapChainExtent = {0, 0};
	std::vector<vk::ImageView> m_SwapChainImageViews;
	std::vector<vk::Framebuffer> m_SwapChainFramebuffers;

	vk::CommandPool m_CommandPool;
	std::vector<vk::CommandBuffer> m_CommandBuffers;
	uint32_t m_CurrentFrame = 0;

	vk::Buffer m_VertexBuffer;
	vk::DeviceMemory m_VertexBufferMemory;

	vk::Buffer m_IndexBuffer;
	vk::DeviceMemory m_IndexBufferMemory;

	vk::DescriptorPool m_DescriptorPool;
	std::vector<vk::DescriptorSet> m_DescriptorSets;

	std::vector<vk::Buffer> m_UniformBuffers;
	std::vector<vk::DeviceMemory> m_UniformBuffersMemory;
	std::vector<void*> m_UniformBuffersMapped;

	// Synchronization
	std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
	std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
	std::vector<vk::Fence> m_InFlightFences;

	ValidationLayer* m_ValidationLayer;
};