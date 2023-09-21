#pragma once
#include <vulkan/vulkan.hpp>
#include <memory.h>
#include "Device.h"

class DescriptorSetLayout
{
public:
	class Builder
	{
	public:
		Builder(Device &device) : m_Device{device} {}

		Builder &AddBinding(
			uint32_t binding,
			vk::DescriptorType descriptorType,
			vk::ShaderStageFlags stageFlags,
			uint32_t count = 1);
		std::unique_ptr<DescriptorSetLayout> Build() const;

	private:
		Device& m_Device;
		std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> m_Bindings{};
	};

	DescriptorSetLayout(
		Device &device,
		std::unordered_map<uint32_t,
		vk::DescriptorSetLayoutBinding> bindings);
	~DescriptorSetLayout();

	DescriptorSetLayout(const DescriptorSetLayout &) = delete;
	DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

	vk::DescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

private:
	Device& m_Device;
	vk::DescriptorSetLayout m_DescriptorSetLayout;
	std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> m_Bindings;

friend class DescriptorWriter;
};

class DescriptorPool
{
public:
	class Builder
	{
	public:
		Builder(Device& device) : m_Device{device} {}

		Builder &AddPoolSize(vk::DescriptorType descriptorType, uint32_t count);
		Builder &SetPoolFlags(vk::DescriptorPoolCreateFlags flags);
		Builder &SetMaxSets(uint32_t count);
		std::unique_ptr<DescriptorPool> Build() const;

	private:
		Device& m_Device;
		std::vector<vk::DescriptorPoolSize> m_PoolSizes{};
		uint32_t m_MaxSets = 1000;
		vk::DescriptorPoolCreateFlags m_PoolFlags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	};

	DescriptorPool(
		Device& device,
		uint32_t maxSets,
		vk::DescriptorPoolCreateFlags poolFlags,
		const std::vector<vk::DescriptorPoolSize> &poolSizes);
	~DescriptorPool();

	DescriptorPool(const DescriptorPool &) = delete;
	DescriptorPool &operator=(const DescriptorPool &) = delete;

	bool AllocateDescriptor(const vk::DescriptorSetLayout descriptorSetLayout, vk::DescriptorSet &descriptor) const;
	void FreeDescriptors(std::vector<vk::DescriptorSet> &descriptors) const;
	void ResetPool();

private:
	Device& m_Device;
	vk::DescriptorPool m_DescriptorPool;

friend class DescriptorWriter;
};

class DescriptorWriter
{
public:
	DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);
	
	DescriptorWriter &WriteBuffer(uint32_t binding, vk::DescriptorBufferInfo* bufferInfo);
	DescriptorWriter &WriteImage(uint32_t binding, vk::DescriptorImageInfo* imageInfo);
	
	bool Build(vk::DescriptorSet& set);
	void Overwrite(vk::DescriptorSet& set);
 
private:
	DescriptorSetLayout& m_SetLayout;
	DescriptorPool& m_Pool;
	std::vector<vk::WriteDescriptorSet> m_Writes;
};