#include "Model.h"

Model::Model(std::string name, const MeshData& meshData, MaterialData material)
    : m_Name(name), m_MeshData(meshData), m_Material(material)
{
}

Model::~Model()
{
}

glm::mat4 Model::GetTranslationMatrix() const
{
    return glm::translate(glm::mat4(1.0f), Position);
}

glm::mat4 Model::GetRotationMatrix() const
{
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotation = glm::rotate(rotation, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    rotation = glm::rotate(rotation, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    return rotation;
}

glm::mat4 Model::GetScaleMatrix() const
{
    return glm::scale( glm::mat4(1.0f), Scale);
}

glm::mat4 Model::GetModelMatrix() const
{
    return GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
}

glm::mat4 Model::GetNormalMatrix() const
{
    return glm::transpose(glm::inverse(GetModelMatrix()));
}
