#include "Buffer.h"

Buffer::Buffer(
    Device& device,
    vk::DeviceSize instanceSize,
    uint32_t instanceCount,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::DeviceSize minOffsetAlignment)
    : m_Device(device), m_InstanceCount(instanceCount), m_InstanceSize(instanceSize), m_Usage(usage), m_MemoryProperties(properties)
{
    m_AlignmentSize = GetAlignment(m_InstanceSize, minOffsetAlignment);
    m_BufferSize = m_AlignmentSize * m_InstanceCount;
    m_Buffer = m_Device.CreateBuffer(m_BufferSize, m_Usage, m_MemoryProperties, m_Memory);
}

Buffer::~Buffer()
{
    Unmap();
    m_Device.GetDevice().destroyBuffer(m_Buffer);
    m_Device.GetDevice().freeMemory(m_Memory);
}

vk::Result Buffer::Map(vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(m_Buffer && m_Memory && "Called map on buffer before create");
    return m_Device.GetDevice().mapMemory(m_Memory, offset, size, vk::MemoryMapFlags(), &m_Mapped);
}

void Buffer::Unmap()
{
    if (m_Mapped)
    {
        m_Device.GetDevice().unmapMemory(m_Memory);
        m_Mapped = nullptr;
    }
}

void Buffer::WriteToBuffer(void *data, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(m_Mapped && "Called write on buffer before map");
    if (size == VK_WHOLE_SIZE)
        memcpy(m_Mapped, data, m_BufferSize);
    else
    {
        char *memOffset = (char *)m_Mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

vk::Result Buffer::Flush(vk::DeviceSize size, vk::DeviceSize offset)
{
    vk::MappedMemoryRange mappedRange(m_Memory, offset, size);
    return m_Device.GetDevice().flushMappedMemoryRanges(1, &mappedRange);
}

vk::DescriptorBufferInfo Buffer::DescriptorInfo(vk::DeviceSize size, vk::DeviceSize offset)
{
    return vk::DescriptorBufferInfo(m_Buffer, offset, size);
}

vk::Result Buffer::Invalidate(vk::DeviceSize size, vk::DeviceSize offset)
{
    vk::MappedMemoryRange mappedRange(m_Memory, offset, size);
    return m_Device.GetDevice().invalidateMappedMemoryRanges(1, &mappedRange);
}

void Buffer::WriteToIndex(void *data, int index)
{
    WriteToBuffer(data, m_InstanceSize, m_AlignmentSize * index);
}

vk::Result Buffer::FlushIndex(int index)
{
    return Flush(m_InstanceSize, m_AlignmentSize * index);
}

vk::DescriptorBufferInfo Buffer::DescriptorInfoForIndex(int index)
{
    return DescriptorInfo(m_InstanceSize, m_AlignmentSize * index);
}

vk::Result Buffer::InvalidateIndex(int index)
{
    return Invalidate(m_InstanceSize, m_AlignmentSize * index);
}

vk::DeviceSize Buffer::GetAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment)
{
    vk::DeviceSize alignment = instanceSize;
    if (minOffsetAlignment > 0)
        alignment = (alignment + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    return alignment;
}
