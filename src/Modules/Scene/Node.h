#pragma once
#include <memory.h>
#include <stack>
#include "./Lighting/DirectionalLight.h"
#include "../Renderer/Vulkan/Mesh.h"
#include "../Renderer/Vulkan/Material.h"
#include "Model.h"
#include "Transform.h"

enum class NodeType
{
    Inner,
    Model,
    Light
};

class Node
{
public:
    Node() = default;
    Node(std::string name);
    Node(std::string name, Model model);
    Node(std::string name, DirectionalLight light);
    void Destroy();

    std::string GetName() const { return m_Name; }
    Model& GetModel() { return m_Model; }
    DirectionalLight& GetLight() { return m_Light; }
    Transform& GetTransform() { return m_Transform; }
    NodeType GetType() const { return m_Type; }

    Node* GetParent() const { return m_Parent; }
    std::vector<Node>& GetChildren() { return m_Children; }

    Node& AddNode(Node& node);

    Node& operator[](uint32_t index) { return m_Children[index]; }
    Node& operator[](std::string name) { return FindNode(name); }

    void OnGUI();
    
private:
    bool NodeExists(std::string name);
    Node& FindNode(std::string name);

    std::string m_Name;
    Model m_Model;
    Transform m_Transform;
    NodeType m_Type = NodeType::Inner;

    Node* m_Parent = nullptr;
    std::vector<Node> m_Children; 
    
    DirectionalLight m_Light;

    Mesh* m_Mesh = nullptr;
    Material* m_Material = nullptr;

friend class Renderer;
};