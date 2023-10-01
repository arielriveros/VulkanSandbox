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

void SceneGraph::AddNode(std::string name, const MeshData &meshData, MaterialData material)
{
    Model model = Model(meshData, material);
    m_Nodes.push_back({ name, model });
}

Node& SceneGraph::FindNode(std::string name)
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
