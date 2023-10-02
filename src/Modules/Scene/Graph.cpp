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
}

void SceneGraph::OnGUI()
{
    for (auto& node : m_Nodes)
        node.OnGUI();
}

void SceneGraph::AddNode(std::string name, const Model& model)
{
    m_Nodes.push_back({ name, model });
}

void SceneGraph::AddNode(std::string name, const DirectionalLight &light)
{
    m_Nodes.push_back({ name, light });
}

std::vector<Node> SceneGraph::GetNodesByType(NodeType type)
{
    std::vector<Node> nodes;
    for (auto& node : m_Nodes)
    {
        if (node.GetType() == type)
            nodes.push_back(node);
    }
    return nodes;
}

Node &SceneGraph::FindNode(std::string name)
{
    for (auto& node : m_Nodes)
    {
        if (node.GetName() == name)
            return node;
    }
    if (m_Nodes.size() > 0)
        return m_Nodes[0];
    else
        throw std::runtime_error("No nodes in scene graph");
}
