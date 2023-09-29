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
    ImGui::Begin("Scene Graph");
    for (auto& model : m_Models)
    {
        model.OnGUI();
    }
    ImGui::End();
}

void SceneGraph::AddNode(std::string name, const MeshData &meshData, MaterialData material)
{
    Model model = Model(name, meshData, material);
    m_Models.push_back(model);
}
