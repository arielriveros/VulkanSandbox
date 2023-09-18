#include "Mesh.h"

Descriptions Vertex::GetDescriptions()
{
	Descriptions descriptions;
	descriptions.BindingDescription = GetBindingDescription();
	descriptions.AttributeDescriptions = GetAttributeDescriptions();
	return descriptions;
}

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
		vk::Format::eR32G32B32Sfloat,	// format
		offsetof(Vertex, Color)			// offset
	));
	attributeDescriptions.push_back(vk::VertexInputAttributeDescription(
		2,								// location
		0,								// binding
		vk::Format::eR32G32Sfloat,		// format
		offsetof(Vertex, TexCoord)		// offset
	));
	return attributeDescriptions;
}

Shape Shape::Triangle()
{
    return Shape({
		{{-0.5f, -0.5f, 0.0f}, Color::red(), {0.0f, 1.0f}},
		{{0.5f, -0.5f, 0.0f}, Color::green(), {1.0f, 1.0f}},
		{{0.0f, 0.5f, 0.0f}, Color::blue(), {0.5f, 0.0f}}},
		{0, 1, 2}
	);
}

Shape Shape::Cube()
{
    return Shape({
		// Front face
		{{-0.5f, -0.5f, 0.5f}, Color::blue(), {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, 0.5f}, Color::blue(), {0.0f, 1.0f}},
		{{ 0.5f,  0.5f, 0.5f}, Color::blue(), {0.0f, 0.0f}},
		{{-0.5f,  0.5f, 0.5f}, Color::blue(), {1.0f, 0.0f}},
		// Back face
		{{ 0.5f, -0.5f, -0.5f}, Color::yellow(), {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}, Color::yellow(), {0.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, Color::yellow(), {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, Color::yellow(), {1.0f, 0.0f}},
		// Left face
		{{-0.5f, -0.5f, -0.5f}, Color::cyan(), {1.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f}, Color::cyan(), {0.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, Color::cyan(), {0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, Color::cyan(), {1.0f, 0.0f}},
		// Right face
		{{0.5f, -0.5f,  0.5f}, Color::red(), {1.0f, 1.0f}},
		{{0.5f, -0.5f, -0.5f}, Color::red(), {0.0f, 1.0f}},
		{{0.5f,  0.5f, -0.5f}, Color::red(), {0.0f, 0.0f}},
		{{0.5f,  0.5f,  0.5f}, Color::red(), {1.0f, 0.0f}},
		// Top face
		{{-0.5f,  0.5f,  0.5f}, Color::green(), {0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f}, Color::green(), {1.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, Color::green(), {1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, Color::green(), {0.0f, 0.0f}},
		// Bottom face
		{{ 0.5, -0.5,  0.5}, Color::magenta(), {1.0f, 1.0f}},
		{{-0.5, -0.5,  0.5}, Color::magenta(), {0.0f, 1.0f}},
		{{-0.5, -0.5, -0.5}, Color::magenta(), {1.0f, 0.0f}},
		{{ 0.5, -0.5, -0.5}, Color::magenta(), {0.0f, 0.0f}}},
		{0, 1, 2, 2, 3, 0,		// Front face
		4, 5, 6, 6, 7, 4,		// Back face
		8, 9, 10, 10, 11, 8,	// Left face
		12, 13, 14, 14, 15, 12, // Right face
		16, 17, 18, 18, 19, 16, // Top face
		20, 21, 22, 22, 23, 20  // Bottom face
		}
	);
}

Mesh::Mesh(Device &device): m_Device(device)
{
}

Mesh::~Mesh()
{
}

void Mesh::Create(Shape shape)
{
	CreateVertexBuffer(shape.Vertices);
	CreateIndexBuffer(shape.Indices);
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
