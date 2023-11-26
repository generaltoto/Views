#include "VGMaterialParticle.h"

#include "VGShaderParticle.h"
#include "D3D/Base/VGHandler.h"

VGMaterialParticle::VGMaterialParticle() : VGMaterial(), m_Color1(), m_Color2(), m_AgeRatio(0.0f)
{
    m_Color1 = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    m_Color2 = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

VGMaterialParticle::~VGMaterialParticle()
{
    VGMaterial::OnDestroy();
}

void VGMaterialParticle::Update()
{
    const auto s = GetShader<VGShaderParticle>();

    auto objC = s->GetNewObjectData();
    objC.World = *m_Renderer->transform->GetTransposedParentedWorldMatrix();
    objC.Color1 = m_Color1;
    objC.Color2 = m_Color2;
    objC.AgeRatio = m_AgeRatio;
    s->UpdateObjectCb(&objC, m_ObjectCbIndex);
}

void VGMaterialParticle::Render()
{
    const auto s = GetShader<VGShaderParticle>();
    ID3D12GraphicsCommandList* cmdList = I(VGHandler)->GetCommandList();

    s->BeginDraw(cmdList);
    s->Draw(cmdList, m_Renderer);
    s->EndDraw(cmdList);

    NULLPTR(cmdList)
}
