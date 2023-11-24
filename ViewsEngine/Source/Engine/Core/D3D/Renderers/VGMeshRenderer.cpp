#include "Engine/ECS/Components/Component.h"

#include "D3D/Shaders/VGMaterial.h"
#include "D3D/Shaders/VGShaderBase.h"

#include "VGMeshRenderer.h"

using namespace DirectX;

VGMeshRenderer::VGMeshRenderer(): VGIRenderer(), m_IsClippable(true)
{
}

VGMeshRenderer::~VGMeshRenderer() = default;

void VGMeshRenderer::Render(ID3D12GraphicsCommandList* cmdList)
{
    if (!IsEnabled() || !m_Mat || !m_Mesh) return;
    const auto shader = m_Mat->GetShader();
    
    shader->BeginDraw(cmdList);

    shader->Draw(cmdList, this);

    shader->EndDraw(cmdList);
}

void VGMeshRenderer::Update(float dt)
{
    if (!IsEnabled() || !m_Mat || !m_Mesh) return;

    transform->UpdateParentedWorldMatrix();
    m_Mat->Update();

    /*
    // First, get the shader that is used by the material.
    // Then, check if the shader is supported by this renderer.
    const auto shader = m_Mat->GetShader();

    if (const auto shaderTexture = dynamic_cast<VGShaderTexture*>(shader))
    {
        auto objC = shaderTexture->GetNewObjectData();
        objC.World = *transform->GetTransposedParentedWorldMatrix();
        shaderTexture->UpdateObjectCb(objC, m_ObjectCbIndex);
    }
    else if (const auto shaderColor = dynamic_cast<VGShaderColor*>(shader))
    {
        auto objC = shaderColor->GetNewObjectData();
        objC.World = *transform->GetTransposedParentedWorldMatrix();
        objC.Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
        shaderColor->UpdateObjectCb(objC, m_ObjectCbIndex);
    }
    else
    {
        throw std::exception("Shader not supported by this renderer.");
    }
    */
}
