#pragma once

#include "D3D/Shaders/Shaders/VGShaderBase.h"

class VGMaterial
{
public:
	VGMaterial();
	~VGMaterial();

	std::string Name;

	void SetShader(VGShaderBase<>* shader);
	[[nodiscard]] VGShaderBase<>* GetShader() const { return m_Shader; }

private:
	VGShaderBase<>* m_Shader;
};