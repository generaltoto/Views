#include "VGMaterialColor.h"

#include "VGShaderColor.h"
#include "D3D/Base/VGHandler.h"

VGMaterialColor::VGMaterialColor() : VGMaterial()
{
    SetName("MaterialColor");
    m_Color = XMFLOAT4(0.2f, 0.8f, 0.2f, 1.0f);
}

VGMaterialColor::~VGMaterialColor()
{
    VGMaterial::OnDestroy();
}

void VGMaterialColor::Update()
{
    const auto s = GetShader<VGShaderColor>();

    auto objC = s->GetNewObjectData();
    objC.World = *m_Renderer->transform->GetTransposedParentedWorldMatrix();
    objC.Color = m_Color;
    s->UpdateObjectCb(&objC, m_ObjectCbIndex);
}

void VGMaterialColor::Render()
{
    const auto s = GetShader<VGShaderColor>();
    ID3D12GraphicsCommandList* cmdList = I(VGHandler)->GetCommandList();

    s->BeginDraw(cmdList);
    s->Draw(cmdList, m_Renderer);
    s->EndDraw(cmdList);

    NULLPTR(cmdList)
}

