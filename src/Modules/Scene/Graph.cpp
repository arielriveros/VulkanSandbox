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
        node.Model.OnGUI(node.Name);
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
        if (node.Name == name)
            return node;
    }
    return Node();
}
