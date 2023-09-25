#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
}

DirectionalLight::DirectionalLight(glm::vec3 position, glm::vec3 color, float intensity)
    : Position(position), Color(color), Intensity(intensity)
{
}