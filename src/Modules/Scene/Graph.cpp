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

void SceneGraph::AddNode(Node *node)
{
    m_Root.AddNode(node);
}

SceneGraphDFSIterator SceneGraph::begin()
{
    return SceneGraphDFSIterator(&m_Root);
}

SceneGraphDFSIterator SceneGraph::end()
{
    return SceneGraphDFSIterator(nullptr);
}

SceneGraphDFSIterator::SceneGraphDFSIterator(Node *node)
{
    if (node != nullptr)
    {
        stack_.push(node);
        AdvanceToNextNode();
    }
}

SceneGraphDFSIterator &SceneGraphDFSIterator::operator++()
{
    AdvanceToNextNode();
    return *this;
}

Node &SceneGraphDFSIterator::operator*()
{
    return *current_node_;
}

bool SceneGraphDFSIterator::operator==(const SceneGraphDFSIterator &other) const
{
    return current_node_ == other.current_node_ && stack_ == other.stack_;
}

bool SceneGraphDFSIterator::operator!=(const SceneGraphDFSIterator &other) const
{
    return !(*this == other);
}

void SceneGraphDFSIterator::AdvanceToNextNode()
{
    if (stack_.empty())
    {
        current_node_ = nullptr;
        return;
    }

    current_node_ = stack_.top();
    stack_.pop();

    for (auto it = current_node_->GetChildren().rbegin(); it != current_node_->GetChildren().rend(); ++it)
    {
        stack_.push(*it);
    }
}