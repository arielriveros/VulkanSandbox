#include <cassert>
#include <stdexcept>
#include "Descriptor.h"

DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::AddBinding(
    uint32_t binding,
    vk::DescriptorType descriptorType,
    vk::ShaderStageFlags stageFlags,
    uint32_t count)
{
	assert(m_Bindings.count(binding) == 0 && "Binding already in use");
	vk::DescriptorSetLayoutBinding layoutBinding(
		binding,
		descriptorType,
		count,
		stageFlags
	);
	m_Bindings[binding] = layoutBinding;
	return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const
{
    return std::make_unique<DescriptorSetLayout>(m_Device, m_Bindings);
}

DescriptorSetLayout::DescriptorSetLayout(
	Device &device,
	std::unordered_map<uint32_t,
	vk::DescriptorSetLayoutBinding> bindings) : m_Device{device}, m_Bindings{bindings}
{
	std::vector<vk::DescriptorSetLayoutBinding> layoutBindings{};
	for (auto binding : m_Bindings)
	{
		layoutBindings.push_back(binding.second);
	}
	vk::DescriptorSetLayoutCreateInfo layoutInfo(
		vk::DescriptorSetLayoutCreateFlags(),
		static_cast<uint32_t>(layoutBindings.size()),
		layoutBindings.data()
	);
	vk::Result createLayoutResult = m_Device.GetDevice().createDescriptorSetLayout(&layoutInfo, nullptr, &m_DescriptorSetLayout);
	if (createLayoutResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create descriptor set layout");

}

DescriptorSetLayout::~DescriptorSetLayout()
{
	m_Device.GetDevice().destroyDescriptorSetLayout(m_DescriptorSetLayout);
}

DescriptorPool::Builder &DescriptorPool::Builder::AddPoolSize(vk::DescriptorType descriptorType, uint32_t count)
{
	vk::DescriptorPoolSize poolSize(
		descriptorType,
		count
	);
	m_PoolSizes.push_back(poolSize);
	return *this;
}

DescriptorPool::Builder &DescriptorPool::Builder::SetPoolFlags(vk::DescriptorPoolCreateFlags flags)
{
	m_PoolFlags = flags;
  	return *this;
}

DescriptorPool::Builder &DescriptorPool::Builder::SetMaxSets(uint32_t count)
{
	m_MaxSets = count;
	return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build() const
{
    return std::make_unique<DescriptorPool>(m_Device, m_MaxSets, m_PoolFlags, m_PoolSizes);
}

DescriptorPool::DescriptorPool(Device &device, uint32_t maxSets, vk::DescriptorPoolCreateFlags poolFlags, const std::vector<vk::DescriptorPoolSize> &poolSizes)
	: m_Device{device}
{
	vk::DescriptorPoolCreateInfo poolInfo(
		poolFlags,
		maxSets,
		static_cast<uint32_t>(poolSizes.size()),
		poolSizes.data()
	);
	vk::Result createPoolResult = device.GetDevice().createDescriptorPool(&poolInfo, nullptr, &m_DescriptorPool);
	if (createPoolResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create descriptor pool");
}

DescriptorPool::~DescriptorPool()
{
	m_Device.GetDevice().destroyDescriptorPool(m_DescriptorPool);
}

bool DescriptorPool::AllocateDescriptor(const vk::DescriptorSetLayout descriptorSetLayout, vk::DescriptorSet &descriptor) const
{
	vk::DescriptorSetAllocateInfo allocInfo(
		m_DescriptorPool,
		1,
		&descriptorSetLayout
	);
	vk::Result allocateResult = m_Device.GetDevice().allocateDescriptorSets(&allocInfo, &descriptor);
	if (allocateResult != vk::Result::eSuccess)
		return false;
	return true;
}

void DescriptorPool::FreeDescriptors(std::vector<vk::DescriptorSet> &descriptors) const
{
	vk::Result freeResult = m_Device.GetDevice().freeDescriptorSets(m_DescriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
	if (freeResult != vk::Result::eSuccess)
		throw std::runtime_error("Failed to free descriptor sets");
}

void DescriptorPool::ResetPool()
{
	m_Device.GetDevice().resetDescriptorPool(m_DescriptorPool);
}

DescriptorWriter::DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool)
	: m_SetLayout{setLayout}, m_Pool{pool}
{
}

DescriptorWriter &DescriptorWriter::WriteBuffer(uint32_t binding, vk::DescriptorBufferInfo *bufferInfo)
{
	assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = m_SetLayout.m_Bindings[binding];

	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	vk::WriteDescriptorSet write(
		vk::DescriptorSet(),
		binding,
		0,
		1,
		bindingDescription.descriptorType,
		nullptr,
		bufferInfo,
		nullptr
	);
	m_Writes.push_back(write);
	return *this;
}

DescriptorWriter &DescriptorWriter::WriteImage(uint32_t binding, vk::DescriptorImageInfo *imageInfo)
{
	assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto &bindingDescription = m_SetLayout.m_Bindings[binding];

	assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

	vk::WriteDescriptorSet write(
		vk::DescriptorSet(),
		binding,
		0,
		1,
		bindingDescription.descriptorType,
		imageInfo,
		nullptr,
		nullptr
	);

	m_Writes.push_back(write);
	return *this;
}

bool DescriptorWriter::Build(vk::DescriptorSet &set)
{
	if (!m_Pool.AllocateDescriptor(m_SetLayout.m_DescriptorSetLayout, set))
		return false;

	for (auto &write : m_Writes)
		write.dstSet = set;

	Overwrite(set);
	return true;
}

void DescriptorWriter::Overwrite(vk::DescriptorSet &set)
{
	for (auto &write : m_Writes)
		write.dstSet = set;

	m_Pool.m_Device.GetDevice().updateDescriptorSets(static_cast<uint32_t>(m_Writes.size()), m_Writes.data(), 0, nullptr);	
}
