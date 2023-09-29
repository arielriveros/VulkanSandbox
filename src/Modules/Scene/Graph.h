#pragma once
#include <string>
#include <vector>
#include "../Renderer/Vulkan/Mesh.h"
#include "../Renderer/Vulkan/Material.h"
#include "Model.h"

struct Node
{
    std::string Name;
    Model Model;
};

class SceneGraph
{
public:
    SceneGraph();
    ~SceneGraph();

    void Initialize();
    void Terminate();

    void OnGUI();

    void AddNode(std::string name, const MeshData& meshData, MaterialData material);
    Node& FindNode(std::string name);
    Node& GetNode(uint32_t index) { return m_Nodes[index]; }

private:
    std::vector<Node> m_Nodes;

    friend class Renderer;
};