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

struct MaterialData {
	MaterialParameters Parameters;
	std::string TexturePath = "";
};

class Material {
public:
    Material() = default;
	Material(Device &device);
	~Material();	

private:
    void Create(MaterialData& parameters);
	void UpdateUniformBuffer(MaterialData& parameters);

    Device& m_Device;
	std::unique_ptr<Buffer> MaterialUniformBuffer;
	std::unique_ptr<Texture> BaseTexture;
	vk::DescriptorSet DescriptorSet;
	std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout{};

	friend class Renderer;
};