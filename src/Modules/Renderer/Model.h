#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vulkan/Mesh.h"
#include <string>

class Model
{
public:
    Model() = default;
    Model(std::string name, const MeshData& meshData);
    ~Model();

    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Rotation = glm::vec3(0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);

    glm::mat4 GetTranslationMatrix() const;
    glm::mat4 GetRotationMatrix() const;
    glm::mat4 GetScaleMatrix() const;
    glm::mat4 GetModelMatrix() const;
    glm::mat4 GetNormalMatrix() const;
    MeshData GetMeshData() const { return m_MeshData; }
    std::string GetName() const { return m_Name; }

private:
    std::string m_Name;
    MeshData m_MeshData;
};