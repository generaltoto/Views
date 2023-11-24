#include "VGShaderBase.h"
#include "D3D/Renderers/VGIRenderer.h"

#include "VGMaterial.h"



VGMaterial::VGMaterial() : m_Name("Material"), m_ObjectCbIndex(-1)
{
    m_Shader = nullptr;
    m_Renderer = nullptr;
}

VGMaterial::~VGMaterial()
{
    // We don't own the shader, so we don't delete it
    VGMaterial::OnDestroy();
    m_Shader = nullptr;
    m_Renderer = nullptr;
}

void VGMaterial::OnCreate(VGShader* shader, VGIRenderer* renderer)
{
    assert(shader != nullptr);
    m_Shader = shader;
    m_ObjectCbIndex = m_Shader->Bind()->GetCreatedIndex();

    assert(renderer != nullptr);
    m_Renderer = renderer;
}

void VGMaterial::OnDestroy()
{
    m_Shader->UnBind(m_ObjectCbIndex);
}

void VGMaterial::SetName(const std::string& name)
{
    m_Name = name;
}
