#include "VGMaterialSkybox.h"

#include "VGShaderSkybox.h"
#include "D3D/Base/VGHandler.h"

VGMaterialSkybox::VGMaterialSkybox() : VGMaterial()
{
}

VGMaterialSkybox::~VGMaterialSkybox() = default;

void VGMaterialSkybox::Update()
{
    const auto s = GetShader<VGShaderSkybox>();
    
    auto objC = s->GetNewObjectData();
    objC.World = *m_Renderer->transform->GetTransposedParentedWorldMatrix();
    s->UpdateObjectCb(&objC, m_ObjectCbIndex);
}

void VGMaterialSkybox::Render()
{
    const auto s = GetShader<VGShaderSkybox>();
    ID3D12GraphicsCommandList* cmdList = I(VGHandler)->GetCommandList();

    s->BeginDraw(cmdList);
    s->Draw(cmdList, m_Renderer);
    s->EndDraw(cmdList);

    NULLPTR(cmdList)
}

