#pragma once

class VGShaderBase;

class VGMaterial
{
public:
	VGMaterial();
	~VGMaterial();

	std::string Name;

	void SetShader(VGShaderBase* shader);
	[[nodiscard]] VGShaderBase* GetShader() const { return m_shader; }

private:
	VGShaderBase* m_shader;
};