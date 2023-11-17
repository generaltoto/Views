
#include "D3D/Shaders/Shaders/VGShaderBase.h"

#include "VGMaterial.h"

VGMaterial::VGMaterial() : Name("Material")
{
	m_shader = nullptr;
}

VGMaterial::~VGMaterial()
{
	// We don't own the shader, so we don't delete it
	m_shader = nullptr;
}

void VGMaterial::SetShader(VGShaderBase * shader)
{
	m_shader = shader;
}
