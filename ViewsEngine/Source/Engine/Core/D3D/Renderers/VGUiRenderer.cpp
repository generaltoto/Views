#include "Engine/ECS/Components/Component.h"
#include "D3D/Shaders/Base/VGMaterial.h"
#include "D3D/Shaders/Shaders/VGShaderBase.h"
#include "D3D/Shaders/Shaders/VGShaderTextureUI.h"

#include "VGUiRenderer.h"

using namespace DirectX;

VGUiRenderer::VGUiRenderer()
    : VGIRenderer(), m_uvOffsetY(0)
{
}

VGUiRenderer::~VGUiRenderer() = default;

void VGUiRenderer::SetOffsetY(const float offsetY)
{
    m_uvOffsetY = offsetY;
}

void VGUiRenderer::AddOffsetY(const float offsetY)
{
    SetOffsetY(m_uvOffsetY + offsetY);
}

void VGUiRenderer::Render(ID3D12GraphicsCommandList* cmdList)
{
    if (!IsEnabled() || !m_Mat || !m_Mesh) return;
    const auto shader = m_Mat->GetShader();

    shader->BeginDraw(cmdList);

    shader->Draw(cmdList, this);

    shader->EndDraw(cmdList);
}

void VGUiRenderer::Update(float dt)
{
    if (!IsEnabled() || !m_Mat || !m_Mesh) return;

    transform->UpdateParentedWorldMatrix();

    if (const auto offsetShader = dynamic_cast<VGShaderTextureUI*>(m_Mat->GetShader()))
    {
        auto objC = offsetShader->GetNewObjectData();
        objC.World = *transform->GetTransposedParentedWorldMatrix();
        objC.Offset = {0.0f, m_uvOffsetY};
        objC.Scale = {transform->GetScale().x, transform->GetScale().y};
        offsetShader->UpdateObjectCb(objC, m_ObjectCbIndex);
    }
    else
    {
        throw std::exception("Shader is not a UI shader!");
    }
}
