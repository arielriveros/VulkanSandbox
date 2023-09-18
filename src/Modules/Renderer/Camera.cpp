#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::UpdateRotation()
{
    float pitch = glm::radians(Rotation.x);
    float yaw = glm::radians(Rotation.y);
    float roll = glm::radians(Rotation.z);

    glm::vec3 forward = glm::vec3(
        cos(yaw) * cos(pitch),
        sin(pitch),
        sin(yaw) * cos(pitch)
    );

    m_Forward = glm::normalize(forward);
    m_Right = glm::normalize(glm::cross(m_Forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
}

void Camera::Resize(float width, float height)
{
    m_AspectRatio = width / height;
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + m_Forward, m_Up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), m_AspectRatio, m_Near, m_Far);
    projection[1][1] *= -1;
    return projection;
}