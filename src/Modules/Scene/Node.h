#pragma once
#include "Model.h"
#include "Transform.h"

class Node
{
public:
    Node() = default;
    Node(std::string name, Model model);

    std::string GetName() const { return m_Name; }
    Model& GetModel() { return m_Model; }
    Transform& GetTransform() { return m_Transform; }

    void OnGUI();
    
private:
    std::string m_Name;
    Model m_Model;
    Transform m_Transform;
};