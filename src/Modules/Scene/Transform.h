#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum RotationOrderEnum
{
    XYZ, XZY,
    YXZ, YZX,
    ZXY, ZYX
};

struct Transform
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Rotation = glm::vec3(0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);

    glm::mat4 PreTransform = glm::mat4(1.0f);

    RotationOrderEnum RotationOrder = RotationOrderEnum::XYZ;

    glm::mat4 GetTranslationMatrix() const;
    glm::mat4 GetRotationMatrix() const;
    glm::mat4 GetScaleMatrix() const;
    glm::mat4 GetCompositeMatrix() const;
    
    glm::mat4 GetNormalMatrix() const;

    glm::vec3 GetForward() const;
};