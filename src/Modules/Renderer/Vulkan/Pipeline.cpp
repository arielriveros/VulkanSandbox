#include <fstream>
#include "Pipeline.h"

Pipeline::Pipeline(vk::Device device, vk::RenderPass renderPass, vk::DescriptorSetLayout descriptorSetLayout)
	: m_Device(device), m_RenderPass(renderPass), m_DescriptorSetLayout(descriptorSetLayout) {}

Pipeline::~Pipeline() {}

void Pipeline::Create(const std::string &vertexSource, const std::string &fragmentSource, Descriptions descriptions)
{
	auto vertShaderCode = ReadFile(vertexSource);
	auto fragShaderCode = ReadFile(fragmentSource);

	vk::ShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
	vk::ShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo(
		vk::PipelineShaderStageCreateFlags(),
		vk::ShaderStageFlagBits::eVertex,
		vertShaderModule,
		"main"
	);

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo(
		vk::PipelineShaderStageCreateFlags(),
		vk::ShaderStageFlagBits::eFragment,
		fragShaderModule,
		"main"
	);

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	std::vector<vk::DynamicState> dynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamicState( vk::PipelineDynamicStateCreateFlags(), dynamicStates );

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.setVertexBindingDescriptions( descriptions.BindingDescription );
    vertexInputInfo.setVertexAttributeDescriptions( descriptions.AttributeDescriptions );

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
		vk::PipelineInputAssemblyStateCreateFlags(),
		vk::PrimitiveTopology::eTriangleList
	);

	vk::PipelineViewportStateCreateInfo viewportState(
		vk::PipelineViewportStateCreateFlags(),
		1, nullptr,
		1, nullptr
	);

	vk::PipelineRasterizationStateCreateInfo rasterizer(
		vk::PipelineRasterizationStateCreateFlags(),
		false,
		false,
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eBack,
		vk::FrontFace::eCounterClockwise,
		false,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	);

	vk::PipelineMultisampleStateCreateInfo multisampling(
		vk::PipelineMultisampleStateCreateFlags(),
		vk::SampleCountFlagBits::e1,
		false,
		1.0f,
		nullptr,
		false,
		false
	);

	vk::PipelineColorBlendAttachmentState colorBlendAttachment(
		false,
		vk::BlendFactor::eOne,
		vk::BlendFactor::eZero,
		vk::BlendOp::eAdd,
		vk::BlendFactor::eOne,
		vk::BlendFactor::eZero,
		vk::BlendOp::eAdd,
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	);

	vk::PipelineColorBlendStateCreateInfo colorBlending(
		vk::PipelineColorBlendStateCreateFlags(),
		false,
		vk::LogicOp::eCopy,
		1, &colorBlendAttachment,
		{ 0.0f, 0.0f, 0.0f, 0.0f }
	);

	vk::PipelineDepthStencilStateCreateInfo depthStencil(
		vk::PipelineDepthStencilStateCreateFlags(),
		true,
		true,
		vk::CompareOp::eLess,
		false,
		false,
		{},
		{},
		0.0f,
		1.0f
	);

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
		vk::PipelineLayoutCreateFlags(),
		1, &m_DescriptorSetLayout,
		0, nullptr
	);

	m_Layout = m_Device.createPipelineLayout(pipelineLayoutInfo);

	vk::GraphicsPipelineCreateInfo pipelineInfo(
		vk::PipelineCreateFlags(),
		2,
		shaderStages,
		&vertexInputInfo,
		&inputAssembly,
		nullptr,
		&viewportState,
		&rasterizer,
		&multisampling,
		&depthStencil,
		&colorBlending,
		&dynamicState,
		m_Layout,
		m_RenderPass,
		0
	);

	vk::Result result;
    vk::Pipeline pipeline;
    std::tie(result, pipeline) = m_Device.createGraphicsPipeline( nullptr, pipelineInfo );

	if (result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create graphics pipeline");

	m_Pipeline = pipeline;

	m_Device.destroyShaderModule(vertShaderModule);
	m_Device.destroyShaderModule(fragShaderModule);
}

void Pipeline::Terminate()
{
	m_Device.destroyPipeline(m_Pipeline);
	m_Device.destroyPipelineLayout(m_Layout);
}

void Pipeline::Bind(vk::CommandBuffer commandBuffer)
{
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);
}

vk::ShaderModule Pipeline::CreateShaderModule(const std::vector<char>& code)
{
	vk::ShaderModuleCreateInfo createInfo(
		vk::ShaderModuleCreateFlags(),
		code.size(),
		reinterpret_cast<const uint32_t*>(code.data())
	);

	return m_Device.createShaderModule(createInfo);
}

std::vector<char> Pipeline::ReadFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("Failed to open file");

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}