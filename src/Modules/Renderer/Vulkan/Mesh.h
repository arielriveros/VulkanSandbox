#pragma once
#include <glm/glm.hpp>
#include "Buffer.h"
#include "Device.h"
#include "Pipeline.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 TexCoord;

	static Descriptions GetDescriptions();
	static vk::VertexInputBindingDescription GetBindingDescription();
	static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions();
};

struct Color
{
	static const glm::vec3 red() { return {1.0f, 0.0f, 0.0f}; }
	static const glm::vec3 green() { return {0.0f, 1.0f, 0.0f}; }
	static const glm::vec3 blue() { return {0.0f, 0.0f, 1.0f}; }
	static const glm::vec3 cyan() { return {0.0f, 1.0f, 1.0f}; }
	static const glm::vec3 magenta() { return {1.0f, 0.0f, 1.0f}; }
	static const glm::vec3 yellow() { return {1.0f, 1.0f, 0.0f}; }
};

struct MeshData
{
	std::vector<Vertex> Vertices;
	std::vector<uint16_t> Indices;

	static MeshData Triangle();
	static MeshData Cube();
};

class Mesh
{
public:
    Mesh(Device& device);
    ~Mesh();

    void Create(std::vector<Vertex> vertices, std::vector<uint16_t> indices);
    void Destroy();
    void Bind(vk::CommandBuffer commandBuffer);
	
	vk::Buffer GetVertexBuffer() const { return m_VertexBuffer->GetBuffer(); }
	vk::Buffer GetIndexBuffer() const { return m_IndexBuffer->GetBuffer(); }
	bool IsIndexed() const { return m_IndexCount > 0; }
	uint32_t GetIndexCount() const { return m_IndexCount; }
	uint32_t GetVertexSize() const { return static_cast<uint32_t>(m_Vertices.size()); }


private:
    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    void DestroyVertexBuffer();
    void CreateIndexBuffer(const std::vector<uint16_t>& indices);
    void DestroyIndexBuffer();

    Device& m_Device;
    Buffer* m_VertexBuffer;
	std::vector<Vertex> m_Vertices;
    Buffer* m_IndexBuffer;
	std::vector<uint16_t> m_Indices;
    uint32_t m_IndexCount = 0;
};