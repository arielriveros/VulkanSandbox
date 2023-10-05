#pragma once
#include <string>
#include <vector>
#include <stack>
#include "../Renderer/Vulkan/Mesh.h"
#include "../Renderer/Vulkan/Material.h"
#include "Node.h"

class SceneGraphDFSIterator
{
public:
    SceneGraphDFSIterator(Node* node);
    SceneGraphDFSIterator& operator++();
    Node& operator*();
    bool operator==(const SceneGraphDFSIterator& other) const;
    bool operator!=(const SceneGraphDFSIterator& other) const;

private:
    void AdvanceToNextNode();


    Node* current_node_{};
    std::stack<Node*> stack_;
};

class SceneGraph
{
public:
    SceneGraph();
    ~SceneGraph();

    void Initialize();
    void Terminate();

    void OnGUI();

    void AddNode(Node& node);

    Node& operator[](std::string name) { return m_Root[name]; }

    SceneGraphDFSIterator begin();
    SceneGraphDFSIterator end() ;

private:
    Node m_Root{"Root"};

friend class Renderer;
};