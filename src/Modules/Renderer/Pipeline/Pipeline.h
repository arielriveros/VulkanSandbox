#include <vulkan/vulkan.hpp>
#include <vector>

struct Descriptions
{
	vk::VertexInputBindingDescription BindingDescription;
	std::vector<vk::VertexInputAttributeDescription> AttributeDescriptions;
};

class Pipeline
{
public:
	Pipeline(vk::Device device, vk::RenderPass renderPass, vk::DescriptorSetLayout descriptorSetLayout);
	~Pipeline();

	void Create(
		const std::string& vertexSource, const std::string& fragmentSource, Descriptions descriptions);
	void Destroy();
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
	vk::DescriptorSetLayout m_DescriptorSetLayout;
};
