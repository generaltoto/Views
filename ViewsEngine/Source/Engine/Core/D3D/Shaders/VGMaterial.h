#pragma once

class VGShader;
class VGIRenderer;

class VGMaterial
{
	friend class Resource;
public:
	[[nodiscard]] const std::string& GetName() const { return m_Name; }
	[[nodiscard]] VGIRenderer* GetRenderer() const { return m_Renderer; }
	[[nodiscard]] VGShader* GetShader() const { return m_Shader; }
	[[nodiscard]] UINT GetObjectCbIndex() const { return m_ObjectCbIndex; }
	
	virtual void Update() = 0;

protected:
	VGMaterial();
	virtual void OnCreate(VGShader* shader, VGIRenderer* renderer);
	
	~VGMaterial();
	virtual void OnDestroy();
	
	std::string m_Name;
	void SetName(const std::string& name);
	
	// Ref to the shader that this material uses
	UINT m_ObjectCbIndex;
	VGShader* m_Shader;
	
	// Ref to the renderer that uses this material
	VGIRenderer* m_Renderer;
};