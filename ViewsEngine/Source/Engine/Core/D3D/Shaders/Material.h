#pragma once

class ShaderBase;

class Material
{
public:
	Material();
	~Material();

	std::string Name;

	void SetShader(ShaderBase* shader);
	[[nodiscard]] ShaderBase* GetShader() const { return m_shader; }

private:
	ShaderBase* m_shader;
};