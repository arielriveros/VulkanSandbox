#include <imgui.h>
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
}

DirectionalLight::DirectionalLight(glm::vec3 position, glm::vec3 color, float intensity)
    : Position(position), Color(color), Intensity(intensity)
{
}

void DirectionalLight::OnGUI()
{
    ImGui::Text("Directional Light");

    ImGui::Text("Position");
    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
    ImGui::DragFloat("##DirLight X Pos", &Position.x, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat("##DirLight Y Pos", &Position.y, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat("##DirLight Z Pos", &Position.z, 0.1f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::ColorEdit3("Light Color", (float*)&Color, ImGuiColorEditFlags_NoInputs);
	ImGui::SliderFloat("Ambient Intensity", &AmbientIntensity, 0.f, 1.f);
}
