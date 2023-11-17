#include "Engine/ECS/Components/Component.h"
#include "D3D/Shaders/Material.h"
#include "D3D/Shaders/ShaderBase.h"

#include "VGUiRenderer.h"

using namespace DirectX;

VGUiRenderer::VGUiRenderer() : VGIRenderer(), m_uvOffsetY(0)
{
}

VGUiRenderer::~VGUiRenderer() = default;

void VGUiRenderer::SetOffsetY(const float offsetY)
{
	m_uvOffsetY = offsetY;
	UpdateShader();
}

void VGUiRenderer::AddOffsetY(const float offsetY)
{
	SetOffsetY(m_uvOffsetY + offsetY);
}

void VGUiRenderer::UpdateShader() const
{
	if (!IsEnabled() || !Mat || !Mesh) return;

	if (const auto offsetShader = dynamic_cast<ShaderTextureUI*>(Mat->GetShader()))
	{
		transform->UpdateParentedWorldMatrix();
		offsetShader->UpdateAsOffset(transform->GetTransposedParentedWorldMatrix(), ObjectCbIndex, m_uvOffsetY);
	}
}

void VGUiRenderer::Render(ID3D12GraphicsCommandList* cmdList)
{
	if (!IsEnabled() || !Mat || !Mesh) return;
	const auto shader = Mat->GetShader();
	
	shader->BeginDraw(cmdList);

	shader->Draw(cmdList, this);

	shader->EndDraw(cmdList);

}

void VGUiRenderer::Update(float dt)
{
	if (!IsEnabled() || !Mat || !Mesh) return;

	transform->UpdateParentedWorldMatrix();

	Mat->GetShader()->UpdateObjectCB(transform->GetTransposedParentedWorldMatrix(), ObjectCbIndex);
}
