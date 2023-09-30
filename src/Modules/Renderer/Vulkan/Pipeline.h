#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

struct PipelineConfig
{
	vk::VertexInputBindingDescription BindingDescription;
	std::vector<vk::VertexInputAttributeDescription> AttributeDescriptions;
	uint32_t SetLayoutCount;
	const vk::DescriptorSetLayout* SetLayouts;
	uint32_t PushConstantRangeSize;

	vk::PolygonMode PolygonMode = vk::PolygonMode::eFill;
	vk::PrimitiveTopology Topology = vk::PrimitiveTopology::eTriangleList;
	vk::CullModeFlagBits CullMode = vk::CullModeFlagBits::eBack;
};

class Pipeline
{
public:
	Pipeline(vk::Device device, vk::RenderPass renderPass);
	~Pipeline();

	void Create(
		const std::string& vertexSource,
		const std::string& fragmentSource,
		PipelineConfig config);
	void Terminate();
    void Bind(vk::CommandBuffer commandBuffer);

	vk::Pipeline GetPipeline() const { return m_Pipeline; }
	vk::PipelineLayout GetLayout() const { return m_Layout; }

private:
	std::vector<char> ReadFile(const std::string& filename);
	vk::ShaderModule CreateShaderModule(const std::vector<char>& code);

private:
	vk::Device m_Device;
	vk::Pipeline m_Pipeline;
	vk::PipelineLayout m_Layout;
	vk::RenderPass m_RenderPass;
};
