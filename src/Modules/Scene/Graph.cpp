#include <imgui.h>
#include "Graph.h"

SceneGraph::SceneGraph()
{
}

SceneGraph::~SceneGraph()
{
}

void SceneGraph::Initialize()
{
}

void SceneGraph::Terminate()
{
    for (auto& node : m_Nodes)
        node.Destroy();
}

void SceneGraph::OnGUI()
{
    for (auto& node : m_Nodes)
        node.OnGUI();
}

void SceneGraph::AddNode(const Node& node)
{
    // Check if node already exists
    if (NodeExists(node.GetName()))
        throw std::runtime_error("Node already exists");

    m_Nodes.push_back(node);
}

bool SceneGraph::NodeExists(std::string name)
{
    for (auto& node : m_Nodes)
    {
        if (node.GetName() == name)
            return true;
    }
    return false;
}

Node &SceneGraph::FindNode(std::string name)
{
    if (NodeExists(name))
        for (auto& node : m_Nodes) if (node.GetName() == name) return node;

    throw std::runtime_error("Node does not exist");
}
