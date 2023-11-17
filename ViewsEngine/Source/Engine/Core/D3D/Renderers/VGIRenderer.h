#pragma once

class Material;
class VGMesh;
class Texture;

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

	[[nodiscard]] Texture* GetTexture(const UINT index) const { return m_Textures[index]; }
	[[nodiscard]] std::vector<Texture*> GetTextures() const { return m_Textures; }
	void RegisterTexture(Texture* tex);

public:
	VGMesh* Mesh;
	UINT ObjectCbIndex = -1;

	Material* Mat;

protected:
	void OnDelete() const;
	void BindMaterial(Material* mat);

	virtual void Render(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void Update(float dt) = 0;
protected:
	std::vector<Texture*> m_Textures{};

private:

};