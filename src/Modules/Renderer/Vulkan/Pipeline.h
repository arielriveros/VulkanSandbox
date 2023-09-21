#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

struct VertexDescriptions
{
	vk::VertexInputBindingDescription BindingDescription;
	std::vector<vk::VertexInputAttributeDescription> AttributeDescriptions;
};

class Pipeline
{
public:
	Pipeline(vk::Device device, vk::RenderPass renderPass);
	~Pipeline();

	void Create(
		const std::string& vertexSource,
		const std::string& fragmentSource,
		VertexDescriptions descriptions,
		vk::DescriptorSetLayout descriptorSetLayout,
		uint32_t );
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
