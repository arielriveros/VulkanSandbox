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

    virtual void OnGUI(){};
};