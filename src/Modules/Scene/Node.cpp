#include <imgui.h>
#include "Node.h"

Node::Node(std::string name)
    : m_Name(name), m_Type(NodeType::Inner)
{
}

Node::Node(std::string name, Model model)
    : m_Name(name), m_Model(model), m_Type(NodeType::Model)
{
}

Node::Node(std::string name, DirectionalLight light)
    : m_Name(name), m_Light(light), m_Type(NodeType::Light)
{
}

void Node::Destroy()
{
    if (m_Mesh != nullptr)
    {
        m_Mesh->Destroy();
        delete m_Mesh;
    }
    if (m_Material != nullptr)
    {
        m_Material->Destroy();
        delete m_Material;
    }

    for (auto& child : m_Children)
        child->Destroy();

    m_Children.clear();
}

void Node::AddNode(Node* node)
{
    // Check if node already exists
    if (NodeExists(node->GetName()))
        throw std::runtime_error("Node already exists");

    node->m_Parent = this;
    m_Children.push_back(node);
}

void Node::OnPropertiesGUI()
{
    ImGui::PushID(m_Name.c_str());

    ImGui::SeparatorText(m_Name.c_str());
    ImGui::Separator();
    m_Transform.OnGUI();
    ImGui::Separator();

    if (m_Type == NodeType::Model)
        m_Model.m_Material.OnGUI();

    if (m_Type == NodeType::Light)
        m_Light.OnGUI();

    ImGui::PopID();
}

bool Node::NodeExists(std::string name)
{
    for (Node* node : m_Children)
    {
        if (node->GetName() == name)
            return true;
    }
    return false;
}

Node* Node::FindNode(std::string name)
{
    if (NodeExists(name))
        for (Node* node : m_Children) if (node->GetName() == name) return node;

    throw std::runtime_error("Node does not exist");
}