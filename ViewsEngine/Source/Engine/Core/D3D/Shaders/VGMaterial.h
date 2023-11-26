#pragma once

class VGShader;
class VGIRenderer;

class VGMaterial
{
	friend class Resource;
public:
	[[nodiscard]] const std::string& GetName() const { return m_Name; }
	[[nodiscard]] VGIRenderer* GetRenderer() const { return m_Renderer; }
	[[nodiscard]] UINT GetObjectCbIndex() const { return m_ObjectCbIndex; }
	
	template <typename T>
	requires std::is_base_of_v<VGShader, T>
	[[nodiscard]] T* GetShader() const { return m_Shader<T>; }
	
	// Update the assigned shader's object constant buffer
	virtual void Update() = 0;

	// Draw the assigned geometry with the assigned shader
	virtual void Render() = 0;

protected:
	VGMaterial();
	// Called by the renderer in the Init() method
	virtual void OnCreate(VGShader* shader, VGIRenderer* renderer);
	
	virtual ~VGMaterial();
	virtual void OnDestroy();
	
	std::string m_Name;
	void SetName(const std::string& name);
	
	// Ref to the shader that this material uses
	UINT m_ObjectCbIndex;

	template <typename T>
	requires std::is_base_of_v<VGShader, T>
	T* m_Shader;
	
	// Ref to the renderer that uses this material
	VGIRenderer* m_Renderer;
};