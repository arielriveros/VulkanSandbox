#pragma once
#include "Model.h"
#include "Transform.h"
#include "./Lighting/DirectionalLight.h"

enum class NodeType
{
    Model,
    Light
};

class Node
{
public:
    Node() = default;
    Node(std::string name, Model model);
    Node(std::string name, DirectionalLight light);

    std::string GetName() const { return m_Name; }
    Model& GetModel() { return m_Model; }
    DirectionalLight& GetLight() { return m_Light; }
    Transform& GetTransform() { return m_Transform; }
    NodeType GetType() const { return m_Type; }

    void OnGUI();
    
private:
    std::string m_Name;
    Model m_Model;
    Transform m_Transform;
    DirectionalLight m_Light;
    NodeType m_Type;
};