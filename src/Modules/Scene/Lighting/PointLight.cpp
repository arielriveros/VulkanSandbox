#include "PointLight.h"

PointLight::PointLight()
{
    Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    Specular = glm::vec3(1.0f, 1.0f, 1.0f);
    Ambient = glm::vec3(0.1f, 0.1f, 0.1f);

    Type = LightType::Point;

    Constant = 1.0f;
    Linear = 0.09f;
    Quadratic = 0.032f;
}

void PointLight::OnGUI()
{
    ImGui::Text("Point Light");
    ImGui::ColorEdit3("Diffuse", &Diffuse.x);
    ImGui::ColorEdit3("Specular", &Specular.x);
    ImGui::ColorEdit3("Ambient", &Ambient.x);

    ImGui::DragFloat("Constant", &Constant, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat("Linear", &Linear, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat("Quadratic", &Quadratic, 0.1f, 0.0f, 100.0f);
}