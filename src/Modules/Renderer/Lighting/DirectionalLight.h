#pragma once
#include <glm/glm.hpp>

class DirectionalLight
{
public:
    DirectionalLight();
    DirectionalLight(glm::vec3 position, glm::vec3 color, float intensity);

    glm::vec3 Position = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Color = glm::vec3(1.0f);
    float Intensity = 1.0f;
    float AmbientIntensity = 0.15f;

    void OnGUI();
};