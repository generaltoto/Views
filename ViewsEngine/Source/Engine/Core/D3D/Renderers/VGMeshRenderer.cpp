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
    m_Mat->Render();
}

void VGMeshRenderer::Update(float dt)
{
    if (!IsEnabled() || !m_Mat || !m_Mesh) return;

    transform->UpdateParentedWorldMatrix();
    m_Mat->Update();
}
