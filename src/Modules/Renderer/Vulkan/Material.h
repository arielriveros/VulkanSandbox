#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "Buffer.h"
#include "Descriptor.h"
#include "Device.h"
#include "Texture.h"

struct MaterialParameters {
	glm::vec4 DiffuseColor = glm::vec4(1.0f);
	glm::vec4 SpecularColor = glm::vec4(1.0f, 1.0f, 1.0f, 32.0f); // w = shininess
	glm::vec4 AmbientColor = glm::vec4(1.0f);
};

enum MaterialType {
	None = -1,
	Basic = 0,
	Default = 1,
	Wireframe = 2,
	PBR = 3
};

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

struct MaterialData {
	MaterialParameters Parameters;
	std::string TexturePath = "";
	MaterialType Type = MaterialType::Default;
};

class Material {
public:
    Material() = default;
	Material(Device &device);
	~Material();

	const MaterialType& GetType() const { return m_Type; }

private:
    void Create(MaterialData& parameters);
	void UpdateUniformBuffer(MaterialData& parameters);

    Device& m_Device;
	std::unique_ptr<Buffer> MaterialUniformBuffer;
	std::unique_ptr<Texture> BaseTexture;
	vk::DescriptorSet DescriptorSet;
	MaterialType m_Type;

	friend class Renderer;
};