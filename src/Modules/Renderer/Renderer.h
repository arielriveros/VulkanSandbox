#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define IMGUI_ENABLE_PROFILER
#include <imgui.h>
#include "Vulkan/Buffer.h"
#include "Vulkan/Descriptor.h"
#include "Vulkan/Device.h"
#include "Vulkan/Mesh.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/Texture.h"
#include "Vulkan/ValidationLayer.h"
#include "../Scene/Camera.h"
#include "../Scene/Graph.h"
#include "../Scene/Model.h"
#include "../Scene/Lighting/DirectionalLight.h"
#include "../Scene/Lighting/PointLight.h"
#include "../ModuleInterface.h"
#include "../../Core/Window.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

struct GPUDirectionalLight
{
	glm::vec4 LightDir; 		// Directional light direction, w = unused
	glm::vec4 LightDiffuse;		// Directional light color (rgb), w = unused
	glm::vec4 LightSpecular;	// Directional light color (rgb), w = unused
	glm::vec4 LightAmbient;		// Directional light color (rgb), w = unused
};

struct GPUPointLight
{
	glm::vec4 LightPos; 		// Point light position, w = 1.0
	glm::vec4 LightDiffuse;		// Point light color (rgb), w = unused
	glm::vec4 LightSpecular;	// Point light color (rgb), w = unused
	glm::vec4 LightAmbient;		// Point light color (rgb), w = unused
	glm::vec4 Properties;		// x = constant, y = linear, z = quadratic, w = unused
};

struct SceneUBO
{
	glm::mat4 ViewProjection;	// Camera view and projection matrix
	glm::vec4 CameraPosition;	// Camera position, w = 1.0
	GPUDirectionalLight DirLight;		// Directional light
	GPUPointLight PointLight;			// Point lights
};

struct PushConstantData {
	glm::mat4 Model;
	glm::mat4 Normal;
};

struct FrameData {
	vk::Semaphore PresentSemaphore; 
	vk::Semaphore RenderSemaphore;
	vk::Fence RenderFence;

	vk::CommandBuffer CommandBuffer;

	std::unique_ptr<Buffer> SceneUniformBuffer;
	vk::DescriptorSet SceneDescriptorSet;
};

struct MaterialPipeline
{
	std::unique_ptr<Pipeline> Pipeline;
	std::unique_ptr<DescriptorSetLayout> MaterialDescriptorSetLayout;
};

class Renderer: public IModule
{
public:
	Renderer(Window& window, Camera& camera, SceneGraph& sceneGraph);
	~Renderer();

	void Initialize();
	void Update();
	void WaitIdle();
	void Terminate();
	void Resize(uint32_t width, uint32_t height);

private:
	void SetupMeshes();
	void SetupMaterials();

	void SetupDescriptors();
	void DestroyDescriptors();

	void SetupPipelines();
	void DestroyPipelines();
	void UpdateSceneUBO(uint32_t currentImage);

	void CreateCommandBuffers();
	void CreateSyncObjects();
	void DestroySyncObjects();

	void BeginFrame(uint32_t& imageIndex);
	void EndFrame(uint32_t& imageIndex);
	void DrawFrame();

	void InitImGui();
	void DrawImGui();
	void DestroyImGui();

private:
	uint32_t m_Width, m_Height;
	bool m_FramebufferResized = false;

	Window& m_Window;
	Camera& m_Camera;
	SceneGraph& m_SceneGraph;
	Node* m_SelectedNode = nullptr;

	std::unique_ptr<Device> m_Device;
	std::unique_ptr<SwapChain> m_SwapChain;

	std::unordered_map<MaterialType, MaterialPipeline, EnumClassHash> m_Pipelines;

	std::vector<FrameData> m_Frames = std::vector<FrameData>(MAX_FRAMES_IN_FLIGHT);
	uint32_t m_CurrentFrame = 0;

	std::unique_ptr<DescriptorPool> m_SceneDescriptorPool{};
	std::unique_ptr<DescriptorSetLayout> m_SceneDescriptorSetLayout{};
	std::unique_ptr<DescriptorPool> m_MaterialDescriptorPool{};

	vk::DescriptorPool m_ImguiPool;
};