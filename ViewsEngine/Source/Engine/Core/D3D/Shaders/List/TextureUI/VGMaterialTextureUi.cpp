#include "VGMaterialTextureUi.h"

#include "VGShaderTextureUI.h"
#include "D3D/Base/VGHandler.h"

VGMaterialTextureUi::VGMaterialTextureUi() : VGMaterial()
{
    m_UvOffset = XMFLOAT2(0.0f, 0.0f);
    m_Scale = XMFLOAT2(1.0f, 1.0f);
}

VGMaterialTextureUi::~VGMaterialTextureUi() = default;

void VGMaterialTextureUi::Update()
{
    const auto s = GetShader<VGShaderTextureUI>();

    auto objC = s->GetNewObjectData();
    objC.World = *m_Renderer->transform->GetTransposedParentedWorldMatrix();
    objC.Offset = m_UvOffset;
    objC.Scale = m_Scale;
    s->UpdateObjectCb(&objC, m_ObjectCbIndex);
}

void VGMaterialTextureUi::Render()
{
    const auto s = GetShader<VGShaderTextureUI>();
    ID3D12GraphicsCommandList* cmdList = I(VGHandler)->GetCommandList();

    s->BeginDraw(cmdList);
    s->Draw(cmdList, m_Renderer);
    s->EndDraw(cmdList);
}

