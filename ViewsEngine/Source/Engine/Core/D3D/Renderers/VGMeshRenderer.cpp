#include "Engine/ECS/Components/Component.h"

#include "D3D/Shaders/Base/VGMaterial.h"
#include "D3D/Shaders/Shaders/VGShaderBase.h"

#include "VGMeshRenderer.h"

#include "D3D/Shaders/Shaders/VGShaderColor.h"
#include "D3D/Shaders/Shaders/VGShaderTexture.h"

using namespace DirectX;

VGMeshRenderer::VGMeshRenderer(): VGIRenderer(), m_IsClippable(true)
{
}

VGMeshRenderer::~VGMeshRenderer() = default;

void VGMeshRenderer::Render(ID3D12GraphicsCommandList* cmdList)
{
    if (!IsEnabled() || !Mat || !Mesh) return;
    const auto shader = Mat->GetShader();
    
    shader->BeginDraw(cmdList);

    shader->Draw(cmdList, this);

    shader->EndDraw(cmdList);
}

void VGMeshRenderer::Update(float dt)
{
    if (!IsEnabled() || !Mat || !Mesh) return;

    transform->UpdateParentedWorldMatrix();

    // First, get the shader that is used by the material.
    // Then, check if the shader is supported by this renderer.
    const auto shader = Mat->GetShader();

    // TODO This is a temporary solution. We need to find a better way to handle this.
    if (const auto shaderTexture = dynamic_cast<VGShaderTexture*>(shader))
    {
        auto objC = shaderTexture->GetNewObjectData();
        objC.World = *transform->GetTransposedParentedWorldMatrix();
        shaderTexture->UpdateObjectCb(objC, ObjectCbIndex);
    }
    else if (const auto shaderColor = dynamic_cast<VGShaderColor*>(shader))
    {
        auto objC = shaderColor->GetNewObjectData();
        objC.World = *transform->GetTransposedParentedWorldMatrix();
        objC.Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f); //TODO Store color in material
        shaderColor->UpdateObjectCb(objC, ObjectCbIndex);
    }
    else
    {
        throw std::exception("Shader not supported by this renderer.");
    }
}
