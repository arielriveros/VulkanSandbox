#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vulkan/Mesh.h"

class Model
{
public:
    Model() = default;
    Model(const MeshData& meshData);
    ~Model();

    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Rotation = glm::vec3(0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);

    glm::mat4 GetModelMatrix() const;
    MeshData GetMeshData() const { return m_MeshData; }

private:
    MeshData m_MeshData;
};