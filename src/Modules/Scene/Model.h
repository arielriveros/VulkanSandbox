#pragma once
#include <string>
#include "../Renderer/Vulkan/Mesh.h"
#include "../Renderer/Vulkan/Material.h"
#include "Transform.h"

class Model
{
public:
    Model() = default;
    Model(const MeshData& meshData, MaterialData material);
    ~Model();

    MeshData GetMeshData() const { return m_MeshData; }
    MaterialData& GetMaterialParameters() { return m_Material; }
    void SetMaterialParameters(MaterialData material) { m_Material = material; }

private:
    MeshData m_MeshData;
    MaterialData m_Material;

    friend class Node;
};