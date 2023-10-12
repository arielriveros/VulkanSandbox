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

void Node::OnGUI()
{
    ImGui::PushID(m_Name.c_str());

    ImGui::SeparatorText(m_Name.c_str());
    ImGui::Text("Transformations");

    if (m_Type == NodeType::Model)
    {
        ImGui::PushItemWidth(100);
        ImGui::Text("Position");
        ImGui::PopItemWidth();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::PushItemWidth(50);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
        ImGui::DragFloat(("##"+ m_Name +"X Pos").c_str(), &m_Transform.Position.x, 0.1f);
        ImGui::PopStyleColor(2);
        ImGui::PopItemWidth();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::PushItemWidth(50);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
        ImGui::DragFloat(("##"+ m_Name +"Y Pos").c_str(), &m_Transform.Position.y, 0.1f);
        ImGui::PopStyleColor(2);
        ImGui::PopItemWidth();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::PushItemWidth(50);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
        ImGui::DragFloat(("##"+ m_Name +"Z Pos").c_str(), &m_Transform.Position.z, 0.1f);
        ImGui::PopStyleColor(2);
        ImGui::PopItemWidth();
    }

    ImGui::PushItemWidth(100);
    ImGui::Text("Rotation");
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ m_Name +"X Rot").c_str(), &m_Transform.Rotation.x, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
    ImGui::DragFloat(("##"+ m_Name +"Y Rot").c_str(), &m_Transform.Rotation.y, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushItemWidth(50);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
    ImGui::DragFloat(("##"+ m_Name +"Z Rot").c_str(), &m_Transform.Rotation.z, 1.0f, -180.0f, 180.0f);
    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();

    if (m_Type == NodeType::Model)
    {
        ImGui::PushItemWidth(100);
        ImGui::Text("Scale   ");
        ImGui::PopItemWidth();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::PushItemWidth(50);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(1.0f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(1.0f, 0.25f, 0.25f, 1.0f));
        ImGui::DragFloat(("##"+ m_Name +"X Scl").c_str(), &m_Transform.Scale.x, 0.1f);
        ImGui::PopStyleColor(2);
        ImGui::PopItemWidth();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::PushItemWidth(50);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 1.0f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 1.0f, 0.25f, 1.0f));
        ImGui::DragFloat(("##"+ m_Name +"Y Scl").c_str(), &m_Transform.Scale.y, 0.1f);
        ImGui::PopStyleColor(2);
        ImGui::PopItemWidth();

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::PushItemWidth(50);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::ImColor(0.5f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 1.00f, 1.0f));
        ImGui::DragFloat(("##"+ m_Name +"Z Scl").c_str(), &m_Transform.Scale.z, 0.1f);
        ImGui::PopStyleColor(2);
        ImGui::PopItemWidth();
        ImGui::Separator();
        ImGui::Text("Material");

        if (ImGui::BeginCombo(("##"+ m_Name +"MaterialType").c_str(), m_Model.m_Material.Type == MaterialType::Basic ? "Basic" : m_Model.m_Material.Type == MaterialType::Wireframe ? "Wireframe" : "Default"))
        {
            if (ImGui::Selectable("Basic", m_Model.m_Material.Type == MaterialType::Basic))
                m_Model.m_Material.Type = MaterialType::Basic;
            if (ImGui::Selectable("Wireframe", m_Model.m_Material.Type == MaterialType::Wireframe))
                m_Model.m_Material.Type = MaterialType::Wireframe;
            if (ImGui::Selectable("Default", m_Model.m_Material.Type == MaterialType::Default))
                m_Model.m_Material.Type = MaterialType::Default;
            ImGui::EndCombo();
        }

        switch(m_Model.m_Material.Type)
        {
            case MaterialType::Basic:
                ImGui::Text("Type: Basic");
                ImGui::Text("Color");
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::PushItemWidth(50);
                ImGui::ColorEdit4(("##"+ m_Name +"Diffuse").c_str(), &m_Model.m_Material.Parameters.DiffuseColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::PopItemWidth();

                ImGui::Text(m_Model.m_Material.TexturePath.c_str());
                break;
            case MaterialType::Wireframe:
                ImGui::Text("Type: Wireframe");
                ImGui::Text("Color");
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::PushItemWidth(50);
                ImGui::ColorEdit4(("##"+ m_Name +"Diffuse").c_str(), &m_Model.m_Material.Parameters.DiffuseColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::PopItemWidth();
                break;
            case MaterialType::Default:
                ImGui::Text("Type: Default");
                ImGui::Text("Diffuse");
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::PushItemWidth(50);
                ImGui::ColorEdit4(("##"+ m_Name +"Diffuse").c_str(), &m_Model.m_Material.Parameters.DiffuseColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::PopItemWidth();

                ImGui::Text(m_Model.m_Material.TexturePath.c_str());

                ImGui::Text("Specular");
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::PushItemWidth(50);
                ImGui::ColorEdit3(("##"+ m_Name +"Specular").c_str(), &m_Model.m_Material.Parameters.SpecularColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::PopItemWidth();

                ImGui::Text("Shininess");
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::PushItemWidth(50);
                ImGui::DragFloat(("##"+ m_Name +"Shininess").c_str(), &m_Model.m_Material.Parameters.SpecularColor.a, 0.2f, 0.0f, 256.0f);
                ImGui::PopItemWidth();

                ImGui::Text("Ambient");
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::PushItemWidth(50);
                ImGui::ColorEdit3(("##"+ m_Name +"Ambient").c_str(), &m_Model.m_Material.Parameters.AmbientColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::PopItemWidth();
                break;
        }
    }

    if (m_Type == NodeType::Light)
    {
        m_Light.OnGUI();
    }

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