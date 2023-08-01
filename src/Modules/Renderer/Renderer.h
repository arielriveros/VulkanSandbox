#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <optional>
#include "../ModuleInterface.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

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
	VkSurfaceCapabilitiesKHR Capabilities;
	std::vector<VkSurfaceFormatKHR> Formats;
	std::vector<VkPresentModeKHR> PresentModes;
};

class Renderer: public IModule
{
public:
	Renderer(GLFWwindow* window);
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
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	bool IsDeviceSuitable(VkPhysicalDevice device);
	void CreateDevice();
	void DestroyDevice();
	void CreateSurface();
	void DestroySurface();
	
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR SelectSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D SelectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void CreateSwapChain();
	void DestroySwapChain();
	void CreateImageViews();
	void DestroyImageViews();
	void RecreateSwapChain();

	void CreateGraphicsPipeline();
	void DestroyGraphicsPipeline();
	void DestroyPipelineLayout();
	std::vector<char> ReadFile(const std::string& filename); // Move to specialized class later
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreateRenderPass();
	void DestroyRenderPass();

	void CreateFramebuffers();
	void DestroyFramebuffers();

	void CreateCommandPool();
	void DestroyCommandPool();
	void CreateCommandBuffers();
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	void CreateSyncObjects();
	void DestroySyncObjects();

	void DrawFrame();

	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	void SetupDebugMessenger();
	static VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);

private:
	uint32_t m_Width, m_Height;
	bool m_FramebufferResized = false;

	GLFWwindow* m_WindowRef;

	VkInstance m_Instance;
	VkPhysicalDevice m_PhysicalDevice;
	VkDevice m_Device;
	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;
	QueueFamilyIndices m_QueueFamilies;

	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
	VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
	VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
	VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

	std::vector<VkImage> m_SwapChainImages;
	VkFormat m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D m_SwapChainExtent = {0, 0};
	std::vector<VkImageView> m_SwapChainImageViews;
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;

	VkCommandPool m_CommandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_CommandBuffers;
	uint32_t m_CurrentFrame = 0;

	// Synchronization
	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;

	// Debugging
	VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
};