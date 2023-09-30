#include "Material.h"

Material::Material(Device &device)
    : m_Device(device)
{
}

Material::~Material()
{
    BaseTexture->Destroy();
    BaseTexture.reset();
}

void Material::Create(MaterialData& parameters)
{
    BaseTexture = std::make_unique<Texture>(m_Device);
    if (parameters.TexturePath != "")
        BaseTexture->LoadFromFile(parameters.TexturePath);
    else
    {
        const unsigned char pixels[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFD, 0xFD, 0xFD, 'V', '3', 0x12, 0x0F };
        BaseTexture->LoadFromBuffer({ (void*)pixels }, 1, 1);
    }

    vk::DeviceSize bufferSize = sizeof(MaterialParameters);
    MaterialUniformBuffer = std::make_unique<Buffer>(
			m_Device,
			bufferSize,
			1,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);
	MaterialUniformBuffer->Map();
    m_Type = parameters.Type;
}

void Material::UpdateMaterial(MaterialData& parameters)
{
    // update type
    if (m_Type != parameters.Type)
        m_Type = parameters.Type;

    // update ubo
    MaterialParameters ubo{};
    ubo.DiffuseColor = parameters.Parameters.DiffuseColor;
    ubo.SpecularColor = parameters.Parameters.SpecularColor;
    ubo.AmbientColor = parameters.Parameters.AmbientColor;
	MaterialUniformBuffer->WriteToBuffer(&ubo);
}
