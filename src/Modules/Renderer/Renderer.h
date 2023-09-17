#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vulkan/Device.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/ValidationLayer.h"
#include "../ModuleInterface.h"
#include "../../Window/Window.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 TexCoord;

	static Descriptions GetDescriptions()
	{
		Descriptions descriptions;
		descriptions.BindingDescription = GetBindingDescription();
		descriptions.AttributeDescriptions = GetAttributeDescriptions();
		return descriptions;
	}

	static vk::VertexInputBindingDescription GetBindingDescription()
	{
		return vk::VertexInputBindingDescription(
			0,								// binding
			sizeof(Vertex),					// stride
			vk::VertexInputRate::eVertex	// inputRate
		);
	}

	static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions()
	{
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
		attributeDescriptions.push_back(vk::VertexInputAttributeDescription(
			0,								// location
			0,								// binding
			vk::Format::eR32G32B32Sfloat,	// format
			offsetof(Vertex, Position)		// offset
		));
		attributeDescriptions.push_back(vk::VertexInputAttributeDescription(
			1,								// location
			0,								// binding
			vk::Format::eR32G32B32Sfloat,	// format
			offsetof(Vertex, Color)			// offset
		));
		attributeDescriptions.push_back(vk::VertexInputAttributeDescription(
			2,								// location
			0,								// binding
			vk::Format::eR32G32Sfloat,		// format
			offsetof(Vertex, TexCoord)		// offset
		));
		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;
};

const glm::vec3 red = 	  {1.0f, 0.0f, 0.0f};
const glm::vec3 green =   {0.0f, 1.0f, 0.0f};
const glm::vec3 blue = 	  {0.0f, 0.0f, 1.0f};
const glm::vec3 cyan = 	  {0.0f, 1.0f, 1.0f};
const glm::vec3 magenta = {1.0f, 0.0f, 1.0f};
const glm::vec3 yellow =  {1.0f, 1.0f, 0.0f};

const std::vector<Vertex> vertices = {
    // Front face
    {{-0.5f, -0.5f, 0.5f}, blue, {1.0f, 1.0f}},
    {{ 0.5f, -0.5f, 0.5f}, blue, {0.0f, 1.0f}},
    {{ 0.5f,  0.5f, 0.5f}, blue, {0.0f, 0.0f}},
    {{-0.5f,  0.5f, 0.5f}, blue, {1.0f, 0.0f}},

	// Back face
	{{ 0.5f, -0.5f, -0.5f}, yellow, {1.0f, 1.0f}},
	{{-0.5f, -0.5f, -0.5f}, yellow, {0.0f, 1.0f}},
	{{-0.5f,  0.5f, -0.5f}, yellow, {0.0f, 0.0f}},
	{{ 0.5f,  0.5f, -0.5f}, yellow, {1.0f, 0.0f}},

	// Left face
	{{-0.5f, -0.5f, -0.5f}, cyan, {1.0f, 1.0f}},
	{{-0.5f, -0.5f,  0.5f}, cyan, {0.0f, 1.0f}},
	{{-0.5f,  0.5f,  0.5f}, cyan, {0.0f, 0.0f}},
	{{-0.5f,  0.5f, -0.5f}, cyan, {1.0f, 0.0f}},

	// Right face
	{{0.5f, -0.5f,  0.5f}, red, {1.0f, 1.0f}},
	{{0.5f, -0.5f, -0.5f}, red, {0.0f, 1.0f}},
	{{0.5f,  0.5f, -0.5f}, red, {0.0f, 0.0f}},
	{{0.5f,  0.5f,  0.5f}, red, {1.0f, 0.0f}},

	// Top face
	{{-0.5f,  0.5f,  0.5f}, green, {0.0f, 1.0f}},
	{{ 0.5f,  0.5f,  0.5f}, green, {1.0f, 1.0f}},
	{{ 0.5f,  0.5f, -0.5f}, green, {1.0f, 0.0f}},
	{{-0.5f,  0.5f, -0.5f}, green, {0.0f, 0.0f}},

	// Bottom face
	{{ 0.5, -0.5,  0.5}, magenta, {1.0f, 1.0f}},
	{{-0.5, -0.5,  0.5}, magenta, {0.0f, 1.0f}},
	{{-0.5, -0.5, -0.5}, magenta, {1.0f, 0.0f}},
	{{ 0.5, -0.5, -0.5}, magenta, {0.0f, 0.0f}}
};

const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,		// Front face
        4, 5, 6, 6, 7, 4,		// Back face
        8, 9, 10, 10, 11, 8,	// Left face
        12, 13, 14, 14, 15, 12,	// Right face
        16, 17, 18, 18, 19, 16,	// Top face
        20, 21, 22, 22, 23, 20 	// Bottom face
};

struct FrameData {
	vk::Semaphore PresentSemaphore; 
	vk::Semaphore RenderSemaphore;
	vk::Fence RenderFence;

	vk::CommandBuffer CommandBuffer;
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
	vk::SurfaceFormatKHR SelectSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR SelectSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D SelectSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
	void CreateSwapChain();
	void DestroySwapChain();
	void CreateImageViews();
	void DestroyImageViews();
	void RecreateSwapChain();

	void CreateRenderPass();
	void DestroyRenderPass();

	void CreateFramebuffers();
	void DestroyFramebuffers();

	void CreateDepthResources();
	void DestroyDepthResources();

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

	void CreateTextureImage();
	void DestroyTextureImage();
	void CreateImage(
		uint32_t width,
		uint32_t height,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Image& image,
		vk::DeviceMemory& imageMemory);
	vk::ImageView CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);
	void CreateTextureImageViews();
	void DestroyTextureImageViews();
	void TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
	void CreateTextureSampler();
	void DestroyTextureSampler();

	void CreateDescriptorSetLayout();
	void DestroyDescriptorSetLayout();
	void CreateDescriptorPool();
	void DestroyeDescriptorPool();
	void CreateDescriptorSets();

	void CreateCommandPool();
	void DestroyCommandPool();
	void CreateCommandBuffers();
	void RecordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

	vk::CommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);

	void CreateSyncObjects();
	void DestroySyncObjects();

	void DrawFrame();

private:
	uint32_t m_Width, m_Height;
	bool m_FramebufferResized = false;

	Window* m_Window;
	Device* m_Device;
	Pipeline* m_Pipeline;

	std::vector<FrameData> m_Frames = std::vector<FrameData>(MAX_FRAMES_IN_FLIGHT);
	uint32_t m_CurrentFrame = 0;

	vk::SwapchainKHR m_SwapChain;
	vk::RenderPass m_RenderPass;
	vk::DescriptorSetLayout m_DescriptorSetLayout;

	std::vector<vk::Image> m_SwapChainImages;
	vk::Format m_SwapChainImageFormat;
	vk::Extent2D m_SwapChainExtent = {0, 0};
	std::vector<vk::ImageView> m_SwapChainImageViews;
	std::vector<vk::Framebuffer> m_SwapChainFramebuffers;

	vk::Image m_DepthImage;
	vk::DeviceMemory m_DepthImageMemory;
	vk::ImageView m_DepthImageView;

	vk::CommandPool m_CommandPool;

	vk::Buffer m_VertexBuffer;
	vk::DeviceMemory m_VertexBufferMemory;

	vk::Buffer m_IndexBuffer;
	vk::DeviceMemory m_IndexBufferMemory;

	vk::Image m_TextureImage;
	vk::DeviceMemory m_TextureImageMemory;
	vk::ImageView m_TextureImageView;
	vk::Sampler m_TextureSampler;

	vk::DescriptorPool m_DescriptorPool;
	std::vector<vk::DescriptorSet> m_DescriptorSets;

	std::vector<vk::Buffer> m_UniformBuffers;
	std::vector<vk::DeviceMemory> m_UniformBuffersMemory;
	std::vector<void*> m_UniformBuffersMapped;
};