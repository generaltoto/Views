#include "Engine/Resource.h"

#include "D3D/Geometry/VGMesh.h"
#include "D3D/Geometry/VGGeometryHandler.h"

#include "D3D/Shaders/VGMaterial.h"
#include "D3D/Shaders/Textures/VGTexture.h"

#include "VGIRenderer.h"

VGIRenderer::VGIRenderer()
{
    m_Mesh = nullptr;
    m_Mat = nullptr;
}

VGIRenderer::~VGIRenderer()
{
    OnDelete();
}

void VGIRenderer::Init(const MeshType meshType, const MaterialType matType)
{
    m_Mesh = VGGeometryHandler::GetMesh(meshType);

    // TODO move this to child class
    m_Mat = Resource::CreateMaterial<VGMaterial>(this, matType);
}

VGTexture* VGIRenderer::GetTexture(const UINT index) const
{
    assert(m_Textures.size() > index);
    return m_Textures.at(index);
}

const std::vector<VGTexture*>& VGIRenderer::GetTextures()
{
    return m_Textures;
}

void VGIRenderer::OnDelete()
{
    DELPTR(m_Mat)
    NULLPTR(m_Mesh)
}

void VGIRenderer::RegisterTexture(VGTexture* tex)
{
    assert(tex != nullptr);
    m_Textures.push_back(tex);
}

VGMaterial* VGIRenderer::GetMaterial() const
{
    assert(m_Mat != nullptr, "Material is nullptr, be sure to call the Init() method first");
    return m_Mat;
}

VGMesh* VGIRenderer::GetMesh() const
{
    assert(m_Mesh != nullptr, "Mesh is nullptr, be sure to call the Init() method first");
    return m_Mesh;
}

UINT VGIRenderer::GetObjectCbIndex() const
{
    assert(m_Mat != nullptr, "Material is nullptr, be sure to call the Init() method first");
    return m_Mat->GetObjectCbIndex();
}

void VGIRenderer::BindMaterial(VGMaterial* mat)
{
    m_Mat = mat;
    //m_ObjectCbIndex = m_Mat->GetShader()->Bind()->GetCreatedIndex();
}
