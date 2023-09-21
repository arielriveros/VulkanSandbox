#pragma once
#include <glm/glm.hpp>
#include "Buffer.h"
#include "Device.h"
#include "Pipeline.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	glm::vec3 Normal;

	static VertexDescriptions GetDescriptions();
	static vk::VertexInputBindingDescription GetBindingDescription();
	static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions();
};

struct MeshData
{
	std::vector<Vertex> Vertices;
	std::vector<uint16_t> Indices;

	static MeshData Triangle();
	static MeshData Cube();
	static MeshData Pyramid();
	static MeshData Sphere(uint32_t definition = 36);
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