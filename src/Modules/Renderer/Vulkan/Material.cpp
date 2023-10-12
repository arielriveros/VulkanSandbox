#include <imgui.h>
#include "Material.h"

Material::Material(Device &device)
    : m_Device(device)
{
}

void Material::Destroy()
{
    BaseTexture->Destroy();
    BaseTexture.reset();
}

void Material::Create(MaterialData& parameters)
{
    BaseTexture = std::make_unique<Texture>(m_Device);
    if (parameters.TexturePath != "")
        BaseTexture->LoadFromFile(ASSETS_PATH + parameters.TexturePath);
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

void MaterialData::OnGUI()
{
    ImGui::Text("Material");
    if (ImGui::BeginCombo("Material Type", Type == MaterialType::Basic ? "Basic" : Type == MaterialType::Wireframe ? "Wireframe" : "Default"))
    {
        if (ImGui::Selectable("Basic", Type == MaterialType::Basic))
            Type = MaterialType::Basic;
        if (ImGui::Selectable("Wireframe", Type == MaterialType::Wireframe))
            Type = MaterialType::Wireframe;
        if (ImGui::Selectable("Default", Type == MaterialType::Default))
            Type = MaterialType::Default;
        ImGui::EndCombo();
    }
    switch(Type)
    {
        case MaterialType::Basic:
            ImGui::Text("Type: Basic");
            ImGui::ColorEdit3("Color", &Parameters.DiffuseColor.x);
            ImGui::Text(TexturePath.c_str());
            break;
        case MaterialType::Wireframe:
            ImGui::Text("Type: Wireframe");
            ImGui::ColorEdit3("Color", &Parameters.DiffuseColor.x);
            break;
        case MaterialType::Default:
            ImGui::Text("Type: Default");
            ImGui::ColorEdit3("Diffuse Color", &Parameters.DiffuseColor.x);
            ImGui::ColorEdit3("Specular Color", &Parameters.SpecularColor.x);
            ImGui::ColorEdit3("Ambient Color", &Parameters.AmbientColor.x);
            ImGui::DragFloat("Shininess", &Parameters.SpecularColor.w, 0.1f, 0.0f, 512.0f);
            break;
    }

    
}
