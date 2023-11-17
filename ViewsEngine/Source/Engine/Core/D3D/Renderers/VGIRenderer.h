#pragma once

class VGMaterial;
class VGMesh;
class VGTexture;

struct ID3D12GraphicsCommandList;

/*
Base class for all Render components
It manages to communication between Shader classes, Mesh classes and D3DRenderer class
*/
class VGIRenderer
{
public:
	VGIRenderer();
	virtual ~VGIRenderer();

public:
	virtual void Init(const MeshType meshType, const MaterialType matType);

	[[nodiscard]] VGTexture* GetTexture(const UINT index) const { return m_Textures[index]; }
	[[nodiscard]] std::vector<VGTexture*> GetTextures() const { return m_Textures; }
	void RegisterTexture(VGTexture* tex);

public:
	VGMesh* Mesh;
	UINT ObjectCbIndex = -1;

	VGMaterial* Mat;

protected:
	void OnDelete() const;
	void BindMaterial(VGMaterial* mat);

	virtual void Render(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void Update(float dt) = 0;
protected:
	std::vector<VGTexture*> m_Textures{};

private:

};