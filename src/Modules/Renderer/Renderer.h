#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vulkan/Buffer.h"
#include "Vulkan/Device.h"
#include "Vulkan/Mesh.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/ValidationLayer.h"
#include "Camera.h"
#include "Model.h"
#include "../ModuleInterface.h"
#include "../../Window/Window.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

struct UniformBufferObject {
	glm::mat4 Model;
	glm::mat4 ViewProjection;
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
	Renderer(Window& window, Camera& camera, Model& model);
	~Renderer();

	void Initialize();
	void Update();
	void WaitIdle();
	void Terminate();
	void Resize(uint32_t width, uint32_t height);

private:
	void SetupModels();
	void RecreateSwapChain();

	void CreateUniformBuffers();
	void DestroyUniformBuffers();
	void UpdateUniformBuffer(uint32_t currentImage);

	void CreateTextureImage();
	void DestroyTextureImage();
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

	void CreateCommandBuffers();
	void RecordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

	void CreateSyncObjects();
	void DestroySyncObjects();

	void DrawFrame();

private:
	uint32_t m_Width, m_Height;
	bool m_FramebufferResized = false;

	Window& m_Window;
	Camera& m_Camera;
	Model& m_Model;

	Device* m_Device;
	Pipeline* m_Pipeline;
	SwapChain* m_SwapChain;

	Mesh* m_Mesh;
	std::vector<Buffer*> m_UniformBuffers;

	std::vector<FrameData> m_Frames = std::vector<FrameData>(MAX_FRAMES_IN_FLIGHT);
	uint32_t m_CurrentFrame = 0;

	vk::Image m_TextureImage;
	vk::DeviceMemory m_TextureImageMemory;
	vk::ImageView m_TextureImageView;
	vk::Sampler m_TextureSampler;

	vk::DescriptorSetLayout m_DescriptorSetLayout;
	vk::DescriptorPool m_DescriptorPool;
	std::vector<vk::DescriptorSet> m_DescriptorSets;
};