#pragma once
#include <string>
#include <vector>
#include <stack>
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

    void AddNode(Node& node);

    Node& operator[](std::string name) { return m_Root[name]; }

private:
    Node m_Root;

friend class Renderer;
friend class SceneGraphDFSIterator;
};