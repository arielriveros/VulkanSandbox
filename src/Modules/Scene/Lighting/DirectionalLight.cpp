#include <imgui.h>
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
    Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    Specular = glm::vec3(1.0f, 1.0f, 1.0f);
    Ambient = glm::vec3(0.1f, 0.1f, 0.1f);

    Type = LightType::Directional;
}

void DirectionalLight::OnGUI()
{
    ImGui::Text("Directional Light");
    ImGui::ColorEdit3("Diffuse", &Diffuse.x);
    ImGui::ColorEdit3("Specular", &Specular.x);
    ImGui::ColorEdit3("Ambient", &Ambient.x);
}
