#include "VGMaterialTexture.h"

#include "VGShaderTexture.h"
#include "D3D/Base/VGHandler.h"

VGMaterialTexture::VGMaterialTexture() : VGMaterial()
{
}

VGMaterialTexture::~VGMaterialTexture() = default;

void VGMaterialTexture::Update()
{
    const auto s = GetShader<VGShaderTexture>();
    
    auto objC = s->GetNewObjectData();
    objC.World = *m_Renderer->transform->GetTransposedParentedWorldMatrix();
    s->UpdateObjectCb(&objC, m_ObjectCbIndex);
}

void VGMaterialTexture::Render()
{
    const auto s = GetShader<VGShaderTexture>();
    ID3D12GraphicsCommandList* cmdList = I(VGHandler)->GetCommandList();

    s->BeginDraw(cmdList);
    s->Draw(cmdList, m_Renderer);
    s->EndDraw(cmdList);

    NULLPTR(cmdList)
}

