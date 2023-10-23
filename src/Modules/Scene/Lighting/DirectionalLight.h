#pragma once
#include <glm/glm.hpp>
#include "Light.h"

class DirectionalLight: public Light
{
public:
    DirectionalLight();
    void OnGUI() override;
};