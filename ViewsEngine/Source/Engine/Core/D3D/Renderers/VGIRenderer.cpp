

#include "Engine/Resource.h"

#include "D3D/Geometry/VGMesh.h"
#include "D3D/Geometry/VGGeometryHandler.h"

#include "D3D/Shaders/Material.h"
#include "D3D/Shaders/ShaderBase.h"
#include "D3D/Shaders/Textures/Texture.h"

#include "VGIRenderer.h"

VGIRenderer::VGIRenderer()
{
	Mesh = nullptr;
	Mat = nullptr;
}

VGIRenderer::~VGIRenderer()
{
	OnDelete();

	Mesh = nullptr;
	Mat = nullptr;
	m_Textures.clear();
}

void VGIRenderer::Init(const MeshType meshType, const MaterialType matType)
{
	Mesh = VGGeometryHandler::GetMesh(meshType);

	Material* mat = Resource::LoadMaterial(matType);
	BindMaterial(mat);
}

void VGIRenderer::OnDelete() const
{
	if (Mat)
		Mat->GetShader()->UnBind(ObjectCbIndex);
}

void VGIRenderer::RegisterTexture(Texture* tex)
{
	assert(tex != nullptr);

	m_Textures.push_back(tex);
}

void VGIRenderer::BindMaterial(Material* mat)
{
	Mat = mat;
	ObjectCbIndex = Mat->GetShader()->Bind()->GetCreatedIndex();
}