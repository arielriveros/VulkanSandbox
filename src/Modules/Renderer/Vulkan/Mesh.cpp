#include "Mesh.h"

vk::VertexInputBindingDescription Vertex::GetBindingDescription()
{
	return vk::VertexInputBindingDescription(
		0,								// binding
		sizeof(Vertex),					// stride
		vk::VertexInputRate::eVertex	// inputRate
	);
}

std::vector<vk::VertexInputAttributeDescription> Vertex::GetAttributeDescriptions()
{
	std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
	attributeDescriptions.push_back(vk::VertexInputAttributeDescription(
		0,								// location
		0,								// binding
		vk::Format::eR32G32B32Sfloat,	// format
		offsetof(Vertex, Position)		// offset
	));
	attributeDescriptions.push_back(vk::VertexInputAttributeDescription(
		1,								// location
		0,								// binding
		vk::Format::eR32G32Sfloat,		// format
		offsetof(Vertex, TexCoord)		// offset
	));
	attributeDescriptions.push_back(vk::VertexInputAttributeDescription(
		2,								// location
		0,								// binding
		vk::Format::eR32G32B32Sfloat,	// format
		offsetof(Vertex, Normal)		// offset
	));
	return attributeDescriptions;
}

MeshData MeshData::Triangle()
{
	MeshData triangle = {};
    triangle.Vertices = {
		{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.0f,  0.5f, 0.0f}, {0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
	};
	triangle.Indices = {0, 1, 2};
	return triangle;
}

MeshData MeshData::Quad()
{
	MeshData quad = {};
	quad.Vertices = {
		{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
	};
	quad.Indices = {0, 1, 2, 2, 3, 0};
	return quad;
}

MeshData MeshData::Cube()
{
	MeshData cube = {};
    cube.Vertices = {
		// Front face
		{{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		// Back face
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		// Left face
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
		// Right face
		{{0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		// Top face
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		// Bottom face
		{{ 0.5, -0.5,  0.5}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		{{-0.5, -0.5,  0.5}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		{{-0.5, -0.5, -0.5}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{ 0.5, -0.5, -0.5}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}
	};
	cube.Indices = {
		0, 1, 2, 2, 3, 0,		// Front face
		4, 5, 6, 6, 7, 4,		// Back face
		8, 9, 10, 10, 11, 8,	// Left face
		12, 13, 14, 14, 15, 12, // Right face
		16, 17, 18, 18, 19, 16, // Top face
		20, 21, 22, 22, 23, 20  // Bottom face
	};
	return cube;
}

MeshData MeshData::Pyramid()
{
    MeshData pyramid = {};
	pyramid.Vertices = {
		// Front face
		{{ 0.0f,  0.5f,  0.0f}, {0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		// Right face
		{{ 0.0f,  0.5f,  0.0f}, {0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		// Back face
		{{ 0.0f,  0.5f,  0.0f}, {0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
		// Left face
		{{ 0.0f,  0.5f,  0.0f}, {0.5f, 0.0f}, {0.0f,  1.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
		// Bottom face
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}
	};
	pyramid.Indices = {
		0, 1, 2,				// Front face
		3, 4, 5,				// Right face
		6, 7, 8,				// Back face
		9, 10, 11,				// Left face
		12, 13, 14, 14, 15, 12	// Bottom face
	};
	return pyramid;
}

MeshData MeshData::Sphere(uint32_t definition)
{
    MeshData sphere = {};

    // Generate vertices
    for (uint32_t i = 0; i <= definition; ++i)
    {
        float v = static_cast<float>(i) / definition;
        float phi = v * 3.141592f;

        for (uint32_t j = 0; j <= definition; ++j)
        {
            float u = static_cast<float>(j) / definition;
            float theta = u * 2 * 3.141592f;

            float x = std::cos(theta) * std::sin(phi);
            float y = std::cos(phi);
            float z = std::sin(theta) * std::sin(phi);

			glm::vec3 position = {x, y, z};
			glm::vec3 normal =  glm::normalize(position);


            sphere.Vertices.push_back({position, {static_cast<float>(definition - j) / definition, v}, normal});
        }
    }

    // Generate indices
	for (uint32_t i = 0; i < definition; ++i)
		for (uint32_t j = 0; j < definition; ++j)
		{
			uint32_t k1 = i * (definition + 1) + j;
			uint32_t k2 = k1 + definition + 1;

			sphere.Indices.push_back(k1);
			sphere.Indices.push_back(k1 + 1);
			sphere.Indices.push_back(k2);

			sphere.Indices.push_back(k2);
			sphere.Indices.push_back(k1 + 1);
			sphere.Indices.push_back(k2 + 1);
		}

    return sphere;
}

Mesh::Mesh(Device &device): m_Device(device)
{
}

void Mesh::Create(std::vector<Vertex> vertices, std::vector<uint16_t> indices)
{
	CreateVertexBuffer(vertices);
	CreateIndexBuffer(indices);
}

void Mesh::Destroy()
{
	DestroyVertexBuffer();
	DestroyIndexBuffer();
}

void Mesh::Bind(vk::CommandBuffer commandBuffer)
{
	vk::Buffer vertexBuffers[] = { m_VertexBuffer->GetBuffer() };
	vk::DeviceSize offsets[] = { 0 };
	commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

	if (m_IndexCount > 0)
		commandBuffer.bindIndexBuffer(m_IndexBuffer->GetBuffer(), 0, vk::IndexType::eUint16);
}

void Mesh::CreateVertexBuffer(const std::vector<Vertex> &vertices)
{
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	m_Vertices = vertices;
	
	Buffer stagingBuffer = Buffer(
		m_Device,
		bufferSize,
		1,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
	);
	stagingBuffer.Map();
	stagingBuffer.WriteToBuffer((void *)vertices.data());

	m_VertexBuffer = new Buffer(
		m_Device,
		bufferSize,
		1,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);

	m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_VertexBuffer->GetBuffer(), bufferSize);
}

void Mesh::DestroyVertexBuffer()
{
	delete m_VertexBuffer;
}

void Mesh::CreateIndexBuffer(const std::vector<uint16_t> &indices)
{
	if (indices.size() <= 0)
		return;

	m_IndexCount = indices.size();
	vk::DeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;
	m_Indices = indices;

	Buffer stagingBuffer = Buffer(
		m_Device,
		bufferSize,
		1,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
	);

	stagingBuffer.Map();
	stagingBuffer.WriteToBuffer((void *)indices.data());

	m_IndexBuffer = new Buffer(
		m_Device,
		bufferSize,
		1,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);

	m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_IndexBuffer->GetBuffer(), bufferSize);
}

void Mesh::DestroyIndexBuffer()
{
	delete m_IndexBuffer;
}
