#include <imgui.h>
#include <imgui_stdlib.h>
#include "Model.h"

Model::Model(const MeshData& meshData, MaterialData material)
    : m_MeshData(meshData), m_Material(material)
{
}

Model::~Model()
{
}