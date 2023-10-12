#pragma once
#include <glm/glm.hpp>
#include <imgui.h>

enum class LightType
{
    Directional,
    Point,
    Spot
};

class Light
{
public:
    Light(){};

    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Ambient;

    LightType Type = LightType::Directional;

    void OnGUI()
    {
        ImGui::ColorEdit3("Diffuse", &Diffuse.x);
        ImGui::ColorEdit3("Specular", &Specular.x);
        ImGui::ColorEdit3("Ambient", &Ambient.x);
    }
};