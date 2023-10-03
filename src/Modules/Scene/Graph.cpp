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
    m_Root.Destroy();
}

void SceneGraph::OnGUI()
{
    m_Root.OnGUI();
}

void SceneGraph::AddNode(Node &node)
{
    m_Root.AddNode(node);
}