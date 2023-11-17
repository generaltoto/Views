#include "Engine/ECS/Components/Component.h"

#include "D3D/Shaders/Material.h"
#include "D3D/Shaders/ShaderBase.h"

#include "VGMeshRenderer.h"

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

    Mat->GetShader()->UpdateObjectCB(transform->GetTransposedParentedWorldMatrix(), ObjectCbIndex);
}
