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
    ImGui::DragFloat(("##"+ id +"X Pos").c_str(), &Position.x, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Y Pos").c_str(), &Position.y, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Z Pos").c_str(), &Position.z, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(100);
    ImGui::Text("Rotation");
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"X Rot").c_str(), &Rotation.x, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Y Rot").c_str(), &Rotation.y, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Z Rot").c_str(), &Rotation.z, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(100);
    ImGui::Text("Scale   ");
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"X Scl").c_str(), &Scale.x, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Y Scl").c_str(), &Scale.y, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat(("##"+ id +"Z Scl").c_str(), &Scale.z, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::Separator();
    ImGui::Text("Material");

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

    ImGui::PopID();
}
