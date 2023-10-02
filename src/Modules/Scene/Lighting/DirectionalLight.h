#pragma once
#include <glm/glm.hpp>

class DirectionalLight
{
public:
    DirectionalLight();
    DirectionalLight(glm::vec3 color, float intensity);

    glm::vec3 Color = glm::vec3(1.0f);
    float Intensity = 1.0f;
    float AmbientIntensity = 0.15f;

    void OnGUI();
};