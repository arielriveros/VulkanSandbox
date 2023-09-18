#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include "Device.h"

class Buffer
{
public:
	Buffer(
		Device& device,
		vk::DeviceSize instanceSize,
		uint32_t instanceCount,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::DeviceSize minOffsetAlignment = 1);
	~Buffer();

	Buffer(const Buffer&) = delete;
	Buffer& operator=(const Buffer&) = delete;

	vk::Result Map(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
	void Unmap();

	void WriteToBuffer(void* data, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
	vk::Result Flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
	vk::DescriptorBufferInfo DescriptorInfo(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
	vk::Result Invalidate(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

	void WriteToIndex(void* data, int index);
	vk::Result FlushIndex(int index);
	vk::DescriptorBufferInfo DescriptorInfoForIndex(int index);
	vk::Result InvalidateIndex(int index);

	vk::Buffer GetBuffer() const { return m_Buffer; }
	void* GetMappedMemory() const { return m_Memory; }
	uint32_t GetInstanceCount() const { return m_InstanceCount; }
	vk::DeviceSize GetInstanceSize() const { return m_InstanceSize; }
	vk::DeviceSize GetAlignmentSize() const { return m_AlignmentSize; }
	vk::BufferUsageFlags GetUsage() const { return m_Usage; }
	vk::MemoryPropertyFlags GetMemoryPropertyFlags() const { return m_MemoryProperties; }
	vk::DeviceSize GetBufferSize() const { return m_BufferSize; }

private:
	static vk::DeviceSize GetAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment);

	Device& m_Device;
	void* m_Mapped = nullptr;
	vk::Buffer m_Buffer;
	vk::DeviceMemory m_Memory;

	vk::DeviceSize m_BufferSize;
	uint32_t m_InstanceCount;
	vk::DeviceSize m_InstanceSize;
	vk::DeviceSize m_AlignmentSize;
	vk::BufferUsageFlags m_Usage;
	vk::MemoryPropertyFlags m_MemoryProperties;
};