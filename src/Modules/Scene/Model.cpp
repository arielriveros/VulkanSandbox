#include <imgui.h>
#include <imgui_stdlib.h>
#include "Model.h"

Model::Model(std::string name, const MeshData& meshData, MaterialData material)
    : m_Name(name), m_MeshData(meshData), m_Material(material)
{
}

Model::~Model()
{
}

glm::mat4 Model::GetTranslationMatrix() const
{
    return glm::translate(glm::mat4(1.0f), Position);
}

glm::mat4 Model::GetRotationMatrix() const
{
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotation = glm::rotate(rotation, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    rotation = glm::rotate(rotation, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    return rotation;
}

glm::mat4 Model::GetScaleMatrix() const
{
    return glm::scale( glm::mat4(1.0f), Scale);
}

glm::mat4 Model::GetModelMatrix() const
{
    return GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
}

glm::mat4 Model::GetNormalMatrix() const
{
    return glm::transpose(glm::inverse(GetModelMatrix()));
}

void Model::OnGUI()
{
    ImGui::PushID(m_Name.c_str());
    ImGui::Text(m_Name.c_str());    

    ImGui::SeparatorText("Transformations");

    ImGui::PushItemWidth(100);
    ImGui::Text("Position");
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+m_Name+"X Pos").c_str(), &Position.x, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+m_Name+"Y Pos").c_str(), &Position.y, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat(("##"+m_Name+"Z Pos").c_str(), &Position.z, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(100);
    ImGui::Text("Rotation");
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+m_Name+"X Rot").c_str(), &Rotation.x, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+m_Name+"Y Rot").c_str(), &Rotation.y, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat(("##"+m_Name+"Z Rot").c_str(), &Rotation.z, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(100);
    ImGui::Text("Scale   ");
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+m_Name+"X Scl").c_str(), &Scale.x, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+m_Name+"Y Scl").c_str(), &Scale.y, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat(("##"+m_Name+"Z Scl").c_str(), &Scale.z, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SeparatorText("Material");

    ImGui::Text("Diffuse");
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::PushItemWidth(50);
    ImGui::ColorEdit4(("##"+m_Name+"Diffuse").c_str(), &m_Material.Parameters.DiffuseColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::PopItemWidth();

    ImGui::Text("Diffuse Texture");
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::PushItemWidth(200);
    ImGui::InputText(("##"+m_Name+"DiffuseTexture").c_str(), &m_Material.TexturePath);

    ImGui::Text("Specular");
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::PushItemWidth(50);
    ImGui::ColorEdit3(("##"+m_Name+"Specular").c_str(), &m_Material.Parameters.SpecularColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::PopItemWidth();

    ImGui::Text("Shininess");
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::PushItemWidth(50);
    ImGui::DragFloat(("##"+m_Name+"Shininess").c_str(), &m_Material.Parameters.SpecularColor.a, 0.2f, 0.0f, 256.0f);
    ImGui::PopItemWidth();

    ImGui::Text("Ambient");
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::PushItemWidth(50);
    ImGui::ColorEdit3(("##"+m_Name+"Ambient").c_str(), &m_Material.Parameters.AmbientColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::PopItemWidth();

    ImGui::Separator();
    ImGui::PopID();
}
