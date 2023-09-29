#pragma once
#include <string>
#include <vector>
#include "../Renderer/Vulkan/Mesh.h"
#include "../Renderer/Vulkan/Material.h"
#include "Model.h"

class SceneGraph
{
public:
    SceneGraph();
    ~SceneGraph();

    void Initialize();
    void Terminate();

    void OnGUI();

    void AddNode(std::string name, const MeshData& meshData, MaterialData material);

private:
    std::vector<Model> m_Models;

    friend class Renderer;
};