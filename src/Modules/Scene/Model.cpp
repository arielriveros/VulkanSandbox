#include <imgui.h>
#include <imgui_stdlib.h>
#include "Model.h"

Model::Model(const MeshData& meshData, MaterialData material)
    : m_MeshData(meshData), m_Material(material)
{
}

Model::~Model()
{
}

void Model::OnGUI(const std::string id)
{
    ImGui::PushID(id.c_str());

    ImGui::SeparatorText(id.c_str());
    ImGui::Text("Transformations");

    ImGui::PushItemWidth(100);
    ImGui::Text("Position");
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"X Pos").c_str(), &Transform.Position.x, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Y Pos").c_str(), &Transform.Position.y, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Z Pos").c_str(), &Transform.Position.z, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(100);
    ImGui::Text("Rotation");
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"X Rot").c_str(), &Transform.Rotation.x, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Y Rot").c_str(), &Transform.Rotation.y, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Z Rot").c_str(), &Transform.Rotation.z, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(100);
    ImGui::Text("Scale   ");
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"X Scl").c_str(), &Transform.Scale.x, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Y Scl").c_str(), &Transform.Scale.y, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Z Scl").c_str(), &Transform.Scale.z, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::Separator();
    ImGui::Text("Material");

    if (ImGui::BeginCombo(("##"+ id +"MaterialType").c_str(), m_Material.Type == MaterialType::Basic ? "Basic" : m_Material.Type == MaterialType::Wireframe ? "Wireframe" : "Default"))
    {
        if (ImGui::Selectable("Basic", m_Material.Type == MaterialType::Basic))
            m_Material.Type = MaterialType::Basic;
        if (ImGui::Selectable("Wireframe", m_Material.Type == MaterialType::Wireframe))
            m_Material.Type = MaterialType::Wireframe;
        if (ImGui::Selectable("Default", m_Material.Type == MaterialType::Default))
            m_Material.Type = MaterialType::Default;
        ImGui::EndCombo();
    }

    switch(m_Material.Type)
    {
        case MaterialType::Basic:
            ImGui::Text("Type: Basic");
            ImGui::Text("Color");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::PushItemWidth(50);
            ImGui::ColorEdit4(("##"+ id +"Diffuse").c_str(), &m_Material.Parameters.DiffuseColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::PopItemWidth();

            ImGui::Text("Texture");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::PushItemWidth(200);
            ImGui::InputText(("##"+ id +"DiffuseTexture").c_str(), &m_Material.TexturePath);
            break;
        case MaterialType::Wireframe:
            ImGui::Text("Type: Wireframe");
            ImGui::Text("Color");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::PushItemWidth(50);
            ImGui::ColorEdit4(("##"+ id +"Diffuse").c_str(), &m_Material.Parameters.DiffuseColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::PopItemWidth();
            break;
        case MaterialType::Default:
            ImGui::Text("Type: Default");
            ImGui::Text("Diffuse");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::PushItemWidth(50);
            ImGui::ColorEdit4(("##"+ id +"Diffuse").c_str(), &m_Material.Parameters.DiffuseColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::PopItemWidth();

            ImGui::Text("Diffuse Texture");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::PushItemWidth(200);
            ImGui::InputText(("##"+ id +"DiffuseTexture").c_str(), &m_Material.TexturePath);

            ImGui::Text("Specular");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::PushItemWidth(50);
            ImGui::ColorEdit3(("##"+ id +"Specular").c_str(), &m_Material.Parameters.SpecularColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::PopItemWidth();

            ImGui::Text("Shininess");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::PushItemWidth(50);
            ImGui::DragFloat(("##"+ id +"Shininess").c_str(), &m_Material.Parameters.SpecularColor.a, 0.2f, 0.0f, 256.0f);
            ImGui::PopItemWidth();

            ImGui::Text("Ambient");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::PushItemWidth(50);
            ImGui::ColorEdit3(("##"+ id +"Ambient").c_str(), &m_Material.Parameters.AmbientColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::PopItemWidth();
            break;
    }

    ImGui::PopID();
}
