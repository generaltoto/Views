#include "VGShaderBase.h"
#include "D3D/Renderers/VGIRenderer.h"

#include "VGMaterial.h"

VGMaterial::VGMaterial()
    : Name("Material")
{
    m_Shader = nullptr;
}

VGMaterial::~VGMaterial()
{
    // We don't own the shader, so we don't delete it
    m_Shader = nullptr;
}

void VGMaterial::SetShader(VGShaderBase<>* shader)
{
    m_Shader = shader;
}
