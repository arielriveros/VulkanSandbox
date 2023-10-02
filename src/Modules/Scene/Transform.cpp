#include "Transform.h"

glm::mat4 Transform::GetTranslationMatrix() const
{
    return glm::translate(glm::mat4(1.0f), Position);
}

glm::mat4 Transform::GetRotationMatrix() const
{
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    switch (RotationOrder)
    {
    case RotationOrderEnum::XYZ:
        return rotationX * rotationY * rotationZ;
    case RotationOrderEnum::XZY:
        return rotationX * rotationZ * rotationY;
    case RotationOrderEnum::YXZ:
        return rotationY * rotationX * rotationZ;
    case RotationOrderEnum::YZX:
        return rotationY * rotationZ * rotationX;
    case RotationOrderEnum::ZXY:
        return rotationZ * rotationX * rotationY;
    case RotationOrderEnum::ZYX:
        return rotationZ * rotationY * rotationX;
    default:
        return glm::mat4(1.0f);
    }
}

glm::mat4 Transform::GetScaleMatrix() const
{
    return glm::scale(glm::mat4(1.0f), Scale);
}

glm::mat4 Transform::GetCompositeMatrix() const
{
    return PreTransform * GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
}

glm::mat4 Transform::GetNormalMatrix() const
{
    return glm::transpose(glm::inverse(GetCompositeMatrix()));
}

glm::vec3 Transform::GetForward() const
{
    glm::vec4 forward = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) * GetRotationMatrix();
    return glm::vec3(forward.x, forward.y, forward.z);
}
