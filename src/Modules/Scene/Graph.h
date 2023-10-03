#pragma once
#include <string>
#include <vector>
#include "../Renderer/Vulkan/Mesh.h"
#include "../Renderer/Vulkan/Material.h"
#include "Node.h"

class SceneGraph
{
public:
    SceneGraph();
    ~SceneGraph();

    void Initialize();
    void Terminate();

    void OnGUI();

    void AddNode(const Node& node);

    Node& operator[](uint32_t index) { return m_Nodes[index]; }
    Node& operator[](std::string name) { return FindNode(name); }

private:
    bool NodeExists(std::string name);
    Node& FindNode(std::string name);
    std::vector<Node> m_Nodes;

friend class Renderer;
};