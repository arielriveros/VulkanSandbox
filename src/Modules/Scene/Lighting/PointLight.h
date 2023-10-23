#pragma once
#include "Light.h"

class PointLight : public Light
{
public:
    PointLight();

    float Constant;
    float Linear;
    float Quadratic;

    void OnGUI() override;
};