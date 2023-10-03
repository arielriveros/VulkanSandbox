#include <iostream>
#include <array>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include "Renderer.h"


Renderer::Renderer(Window& window, Camera& camera, SceneGraph& sceneGraph)
	: m_Window{ window },
	  m_Camera{ camera },
	  m_SceneGraph{ sceneGraph }
{
	std::cout << "Renderer Constructor" << std::endl;
	Resize(m_Window.Width, m_Window.Height);
	m_Camera.UpdateRotation();
}

Renderer::~Renderer()
{
	std::cout << "Renderer Destructor" << std::endl;
}

void Renderer::Initialize()
{
	try
	{
		m_Device = std::make_unique<Device>(m_Window);
		m_Device->Initialize();
		m_SwapChain = std::make_unique<SwapChain>(*m_Device, m_Window);
		m_SwapChain->Initialize();
		SetupDescriptors();
		SetupPipelines();
		SetupMaterials();
		SetupMeshes();
		CreateCommandBuffers();
		CreateSyncObjects();
		InitImGui();
	}
	catch (vk::SystemError& err)
    {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        exit(-1);
    }
}

void Renderer::Update()
{
	try
	{
		DrawFrame();
	}
	catch (vk::SystemError& err)
    {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        exit(-1);
    }
}

void Renderer::WaitIdle()
{
	m_Device->WaitIdle();
}

void Renderer::Terminate()
{
	try
	{
		DestroyImGui();
		m_SceneGraph.Terminate();
		DestroyPipelines();
		DestroyDescriptors();
		DestroySyncObjects();
		m_SwapChain->Terminate();
		m_Device->Terminate();
	}
	catch (vk::SystemError& err)
    {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        exit(-1);
    }
}

void Renderer::Resize(uint32_t width, uint32_t height)
{
	if (m_Width == width && m_Height == height) return;
	m_Width = width;
	m_Height = height;
	m_FramebufferResized = true;
	m_Camera.Resize(m_Width, m_Height);
}

void Renderer::SetupMeshes()
{
	for (Node& node : m_SceneGraph.m_Root.m_Children)
	{
		if (node.GetType() == NodeType::Model)
		{
			MeshData meshData = node.GetModel().GetMeshData();
			node.m_Mesh = new Mesh(*m_Device);
			node.m_Mesh->Create(meshData.Vertices, meshData.Indices);
		}

		for (Node child : node.m_Children)
		{
			if (child.GetType() == NodeType::Model)
			{
				MeshData meshData = child.GetModel().GetMeshData();
				child.m_Mesh = new Mesh(*m_Device);
				child.m_Mesh->Create(meshData.Vertices, meshData.Indices);
			}
		}
	}
}

void Renderer::SetupMaterials()
{
	for (Node& node : m_SceneGraph.m_Root.m_Children)
	{
		if (node.GetType() == NodeType::Model)
		{
			node.m_Material = new Material(*m_Device);
			
			auto parameters = node.GetModel().GetMaterialParameters();
			node.m_Material->Create(parameters);
			
			DescriptorWriter(
				*m_Pipelines[parameters.Type].MaterialDescriptorSetLayout,
				*m_MaterialDescriptorPool)
					.WriteBuffer(0, &node.m_Material->MaterialUniformBuffer->DescriptorInfo())
					.WriteImage(1, &node.m_Material->BaseTexture->DescriptorInfo())
					.Build(node.m_Material->DescriptorSet);
		}

		for (Node child : node.m_Children)
		{
			if (child.GetType() == NodeType::Model)
			{
				child.m_Material = new Material(*m_Device);

				auto parameters = child.GetModel().GetMaterialParameters();
				child.m_Material->Create(parameters);
				
				DescriptorWriter(
					*m_Pipelines[parameters.Type].MaterialDescriptorSetLayout,
					*m_MaterialDescriptorPool)
						.WriteBuffer(0, &child.m_Material->MaterialUniformBuffer->DescriptorInfo())
						.WriteImage(1, &child.m_Material->BaseTexture->DescriptorInfo())
						.Build(child.m_Material->DescriptorSet);
			}
		}
	}
}

void Renderer::SetupDescriptors()
{
	vk::DeviceSize bufferSize = sizeof(SceneUBO);

	m_SceneDescriptorSetLayout = DescriptorSetLayout::Builder(*m_Device)
		.AddBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
		.Build();

	m_SceneDescriptorPool = DescriptorPool::Builder(*m_Device)
		.SetMaxSets(MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eUniformBuffer, 1)
		.Build();

	m_MaterialDescriptorPool = DescriptorPool::Builder(*m_Device)
		.SetMaxSets(1000)
		.AddPoolSize(vk::DescriptorType::eUniformBuffer, 1)
		.AddPoolSize(vk::DescriptorType::eCombinedImageSampler, 1)
		.Build();

	for (uint32_t i = 0; i < m_Frames.size(); i++)
	{
		m_Frames[i].SceneUniformBuffer = std::make_unique<Buffer>(
			*m_Device,
			bufferSize,
			1,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);
		m_Frames[i].SceneUniformBuffer->Map();

		vk::DescriptorBufferInfo bufferInfo = m_Frames[i].SceneUniformBuffer->DescriptorInfo();

		DescriptorWriter (*m_SceneDescriptorSetLayout, *m_SceneDescriptorPool)
			.WriteBuffer(0, &bufferInfo)
			.Build(m_Frames[i].SceneDescriptorSet);
	}
}

void Renderer::DestroyDescriptors()
{
	m_SceneDescriptorPool.reset();
	m_SceneDescriptorSetLayout.reset();
	m_MaterialDescriptorPool.reset();

	for (size_t i = 0; i < m_Frames.size(); i++)
		m_Frames[i].SceneUniformBuffer.reset();
}

void Renderer::SetupPipelines()
{
	// default pipeline
	{
		auto pipeline = Pipeline(m_Device->GetDevice(), m_SwapChain->GetRenderPass());
		auto sceneLayout = m_SceneDescriptorSetLayout->GetDescriptorSetLayout();
		auto materialLayout = DescriptorSetLayout::Builder(*m_Device)
			.AddBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment)
			.AddBinding(1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
			.Build();
			
		std::array<vk::DescriptorSetLayout, 2> setLayouts = { sceneLayout, materialLayout->GetDescriptorSetLayout() };
		pipeline.Create(
			"resources/shaders/default.vert.spv",
			"resources/shaders/default.frag.spv",
			{
				Vertex::GetBindingDescription(),
				Vertex::GetAttributeDescriptions(),
				static_cast<uint32_t>(setLayouts.size()),
				setLayouts.data(),
				sizeof(PushConstantData)
			}
		);

		MaterialPipeline materialPipelineData{};
		materialPipelineData.Pipeline = std::make_unique<Pipeline>(pipeline);
		materialPipelineData.MaterialDescriptorSetLayout = std::move(materialLayout);

		m_Pipelines.insert({ MaterialType::Default, std::move(materialPipelineData) });
	}

	// basic pipeline
	{
		auto pipeline = Pipeline(m_Device->GetDevice(), m_SwapChain->GetRenderPass());
		auto sceneLayout = m_SceneDescriptorSetLayout->GetDescriptorSetLayout();
		auto materialLayout = DescriptorSetLayout::Builder(*m_Device)
			.AddBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment)
			.AddBinding(1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
			.Build();
			
		std::array<vk::DescriptorSetLayout, 2> setLayouts = { sceneLayout, materialLayout->GetDescriptorSetLayout() };
		pipeline.Create(
			"resources/shaders/basic.vert.spv",
			"resources/shaders/basic.frag.spv",
			{
				Vertex::GetBindingDescription(),
				Vertex::GetAttributeDescriptions(),
				static_cast<uint32_t>(setLayouts.size()),
				setLayouts.data(),
				sizeof(PushConstantData)
			}
		);

		MaterialPipeline materialPipelineData{};
		materialPipelineData.Pipeline = std::make_unique<Pipeline>(pipeline);
		materialPipelineData.MaterialDescriptorSetLayout = std::move(materialLayout);

		m_Pipelines.insert({ MaterialType::Basic, std::move(materialPipelineData) });
	}

	// wireframe pipeline
	{
		auto pipeline = Pipeline(m_Device->GetDevice(), m_SwapChain->GetRenderPass());
		auto sceneLayout = m_SceneDescriptorSetLayout->GetDescriptorSetLayout();
		auto materialLayout = DescriptorSetLayout::Builder(*m_Device)
			.AddBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment)
			.AddBinding(1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
			.Build();
			
		std::array<vk::DescriptorSetLayout, 2> setLayouts = { sceneLayout, materialLayout->GetDescriptorSetLayout() };
		pipeline.Create(
			"resources/shaders/basic.vert.spv",
			"resources/shaders/basic.frag.spv",
			{
				Vertex::GetBindingDescription(),
				Vertex::GetAttributeDescriptions(),
				static_cast<uint32_t>(setLayouts.size()),
				setLayouts.data(),
				sizeof(PushConstantData),
				vk::PolygonMode::eLine,
				vk::PrimitiveTopology::eTriangleList,
				vk::CullModeFlagBits::eNone
			}
		);

		MaterialPipeline materialPipelineData{};
		materialPipelineData.Pipeline = std::make_unique<Pipeline>(pipeline);
		materialPipelineData.MaterialDescriptorSetLayout = std::move(materialLayout);

		m_Pipelines.insert({ MaterialType::Wireframe, std::move(materialPipelineData) });
	}
}

void Renderer::DestroyPipelines()
{
	for (auto& pipeline : m_Pipelines)
	{
		pipeline.second.Pipeline->Terminate();
		pipeline.second.MaterialDescriptorSetLayout.reset();
	}
}

void Renderer::UpdateSceneUBO(uint32_t currentImage)
{
	SceneUBO ubo{};
	ubo.ViewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();
	ubo.CameraPosition = glm::vec4(m_Camera.Position, 1.0f);
	ubo.DirectionalLightDirection = glm::vec4(m_SceneGraph["sun"].GetTransform().GetForward(), m_SceneGraph["sun"].GetLight().Intensity);
	ubo.DirectionalLightColor = glm::vec4(m_SceneGraph["sun"].GetLight().Color, m_SceneGraph["sun"].GetLight().AmbientIntensity);

	m_Frames[currentImage].SceneUniformBuffer->WriteToBuffer(&ubo);
}

void Renderer::CreateCommandBuffers()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::CommandBufferAllocateInfo allocInfo(m_Device->GetCommandPool(), vk::CommandBufferLevel::ePrimary, 1);

		vk::Result result = m_Device->GetDevice().allocateCommandBuffers(&allocInfo, &m_Frames[i].CommandBuffer);
		if (result != vk::Result::eSuccess)
			throw std::runtime_error("Failed to allocate command buffers");
	}
}

void Renderer::CreateSyncObjects()
{
	vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_Frames[i].RenderSemaphore = m_Device->GetDevice().createSemaphore(vk::SemaphoreCreateInfo());
		m_Frames[i].PresentSemaphore = m_Device->GetDevice().createSemaphore(vk::SemaphoreCreateInfo());
		m_Frames[i].RenderFence = m_Device->GetDevice().createFence(fenceInfo);

		if (!m_Frames[i].RenderSemaphore || !m_Frames[i].PresentSemaphore || !m_Frames[i].RenderFence)
			throw std::runtime_error("Failed to create sync objects");
	}
}

void Renderer::DestroySyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_Device->GetDevice().destroyFence(m_Frames[i].RenderFence);
		m_Device->GetDevice().destroySemaphore(m_Frames[i].RenderSemaphore);
		m_Device->GetDevice().destroySemaphore(m_Frames[i].PresentSemaphore);
	}
}

void Renderer::DrawFrame()
{
	vk::CommandBuffer& commandBuffer = m_Frames[m_CurrentFrame].CommandBuffer;
	uint32_t currentBuffer{};

	BeginFrame(currentBuffer);	
	UpdateSceneUBO(m_CurrentFrame);

	MaterialType currentPipeline = MaterialType::None;

	for (Node& node: m_SceneGraph.m_Root.m_Children)
	{
		// only draw model type nodes
		if (node.GetType() != NodeType::Model)
			continue;

		// only bind pipeline if it's different from the last one
		if (currentPipeline != node.m_Material->GetType())
		{
			currentPipeline = node.m_Material->GetType();
			m_Pipelines[currentPipeline].Pipeline->Bind(commandBuffer);
			
			// bind scene descriptor set
			commandBuffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			m_Pipelines[node.m_Material->GetType()].Pipeline->GetLayout(),
			0,
			1, &m_Frames[m_CurrentFrame].SceneDescriptorSet,
			0, nullptr);
		}
		
		PushConstantData pushConstantData{};
		pushConstantData.Model = node.GetTransform().GetCompositeMatrix();
		pushConstantData.Normal = node.GetTransform().GetNormalMatrix();
		commandBuffer.pushConstants(m_Pipelines[node.m_Material->GetType()].Pipeline->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstantData), &pushConstantData);

		node.m_Material->UpdateMaterial(node.GetModel().GetMaterialParameters());

		// bind material descriptor set
		commandBuffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			m_Pipelines[node.m_Material->GetType()].Pipeline->GetLayout(),
			1,
			1, &node.m_Material->DescriptorSet,
			0, nullptr);

		node.m_Mesh->Bind(commandBuffer);
		if(node.m_Mesh->IsIndexed())
			commandBuffer.drawIndexed(node.m_Mesh->GetIndexCount(), 1, 0, 0, 0);

		else
			commandBuffer.draw(node.m_Mesh->GetVertexSize(), 1, 0, 0);
	}

	// TODO: move to begin frame function
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	DrawImGui();
	// TODO: move to end frame function
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_Frames[m_CurrentFrame].CommandBuffer);
	EndFrame(currentBuffer);
}

void Renderer::InitImGui()
{
	vk::DescriptorPoolSize pool_sizes[] =
	{
		{ vk::DescriptorType::eSampler, 1000 },
		{ vk::DescriptorType::eCombinedImageSampler, 1000 },
		{ vk::DescriptorType::eSampledImage, 1000 },
		{ vk::DescriptorType::eStorageImage, 1000 },
		{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
		{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
		{ vk::DescriptorType::eUniformBuffer, 1000 },
		{ vk::DescriptorType::eStorageBuffer, 1000 },
		{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
		{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
		{ vk::DescriptorType::eInputAttachment, 1000 }
	};

	vk::DescriptorPoolCreateInfo pool_info(
		vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		1000,
		std::size(pool_sizes),
		pool_sizes
	);

	vk::Result createPoolResult = m_Device->GetDevice().createDescriptorPool(&pool_info, nullptr, &m_ImguiPool);
	if (createPoolResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create descriptor pool");

	ImGui::CreateContext();

	const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_SRGB };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	ImGui_ImplVulkanH_SelectSurfaceFormat(
		m_Device->GetPhysicalDevice(),
		m_Device->GetSurface(),
		requestSurfaceImageFormat,
		(size_t)IM_ARRAYSIZE(requestSurfaceImageFormat),
		requestSurfaceColorSpace
	);

	ImGui_ImplGlfw_InitForVulkan(m_Window.GetWindow(), true);

	ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = m_Device->GetInstance();
		init_info.PhysicalDevice = m_Device->GetPhysicalDevice();
		init_info.Device = m_Device->GetDevice();
		init_info.QueueFamily = m_Device->GetQueueFamilies().GraphicsFamily.value();
		init_info.Queue = m_Device->GetGraphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = m_ImguiPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = ImGui_ImplVulkanH_GetMinImageCountFromPresentMode(static_cast<VkPresentModeKHR>(m_SwapChain->GetPresentMode()));
		init_info.ImageCount = ImGui_ImplVulkanH_GetMinImageCountFromPresentMode(static_cast<VkPresentModeKHR>(m_SwapChain->GetPresentMode()));
		init_info.CheckVkResultFn = nullptr;

	ImGui_ImplVulkan_Init(&init_info, m_SwapChain->GetRenderPass());

	vk::CommandBuffer commandBuffer = m_Device->BeginSingleTimeCommands();

	ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
	m_Device->EndSingleTimeCommands(commandBuffer);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Renderer::DrawImGui()
{
    ImGui::Begin("Scene Hierarchy");
    for (auto& node : m_SceneGraph.m_Root.m_Children)
	{
        if (ImGui::Selectable(node.GetName().c_str(), m_SelectedNode == &node))
			m_SelectedNode = &node;
	}
    ImGui::End();

    ImGui::Begin("Node Properties");
    if (m_SelectedNode != nullptr)
		m_SelectedNode->OnGUI();
    ImGui::End();

	//ImGui::ShowDemoWindow();
	//ImGui::ShowMetricsWindow();
}

void Renderer::DestroyImGui()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	m_Device->GetDevice().destroyDescriptorPool(m_ImguiPool);
}

void Renderer::BeginFrame(uint32_t &imageIndex)
{
	if (m_FramebufferResized)
	{
		while (m_Width == 0 || m_Height == 0)
			glfwWaitEvents();

		m_Device->WaitIdle();
		m_SwapChain->Recreate();
		m_FramebufferResized = false;
	}

	while(vk::Result::eTimeout == m_Device->GetDevice().waitForFences(1, &m_Frames[m_CurrentFrame].RenderFence, VK_TRUE, UINT64_MAX));

	vk::ResultValue<uint32_t> currentBuffer = m_Device->GetDevice().acquireNextImageKHR(m_SwapChain->GetSwapChain(),
																						UINT64_MAX,
																						m_Frames[m_CurrentFrame].PresentSemaphore,
																						nullptr);

	if (currentBuffer.result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to acquire swap chain image");

	imageIndex = currentBuffer.value;

	while (vk::Result::eTimeout == m_Device->GetDevice().resetFences(1, &m_Frames[m_CurrentFrame].RenderFence));
	m_Frames[m_CurrentFrame].CommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

	vk::CommandBufferBeginInfo beginInfo(
		vk::CommandBufferUsageFlagBits::eSimultaneousUse,
		nullptr
	);

	m_Frames[m_CurrentFrame].CommandBuffer.begin(beginInfo);
	
	const vk::ClearValue clearValues[2]{
		{vk::ClearColorValue(std::array<float, 4>{.05f, 0.f, .05f, 1.f})},
		{vk::ClearDepthStencilValue(1.f, 0)}
	};

	vk::RenderPassBeginInfo renderPassInfo(
		m_SwapChain->GetRenderPass(),
		m_SwapChain->GetFramebuffer(imageIndex),
		vk::Rect2D( vk::Offset2D( 0, 0 ), m_SwapChain->GetExtent() ),
		2, clearValues
	);

	m_Frames[m_CurrentFrame].CommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	vk::Viewport viewport(
		0.0f,
		0.0f,
		static_cast<float>(m_SwapChain->GetExtent().width),
		static_cast<float>(m_SwapChain->GetExtent().height),
		0.0f,
		1.0f
	);
	m_Frames[m_CurrentFrame].CommandBuffer.setViewport(0, 1, &viewport);

	vk::Rect2D scissor(vk::Offset2D(0, 0), m_SwapChain->GetExtent());
	m_Frames[m_CurrentFrame].CommandBuffer.setScissor(0, 1, &scissor);
}

void Renderer::EndFrame(uint32_t &imageIndex)
{
	m_Frames[m_CurrentFrame].CommandBuffer.endRenderPass();
	m_Frames[m_CurrentFrame].CommandBuffer.end();

	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

	vk::SubmitInfo submitInfo(
		1, &m_Frames[m_CurrentFrame].PresentSemaphore,
		waitStages,
		1, &m_Frames[m_CurrentFrame].CommandBuffer,
		1, &m_Frames[m_CurrentFrame].RenderSemaphore
	);

	vk::Result queueSubmitResult = m_Device->GetGraphicsQueue().submit(1, &submitInfo, m_Frames[m_CurrentFrame].RenderFence);
	if(queueSubmitResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to submit draw command buffer");

	vk::Result queuePresentResult = m_Device->GetPresentQueue().presentKHR(
		vk::PresentInfoKHR(1, &m_Frames[m_CurrentFrame].RenderSemaphore,
						   1, &m_SwapChain->GetSwapChain(),
						   &imageIndex)
	);
	if (queuePresentResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to present swap chain image");

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
