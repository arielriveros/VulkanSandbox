#include <imgui.h>
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
}

DirectionalLight::DirectionalLight(glm::vec3 color, float intensity)
    : Color(color), Intensity(intensity)
{
}

void DirectionalLight::OnGUI()
{
    ImGui::Text("Directional Light");
    ImGui::ColorEdit3("Light Color", (float*)&Color, ImGuiColorEditFlags_NoInputs);
	ImGui::SliderFloat("Ambient Intensity", &AmbientIntensity, 0.f, 1.f);
}
