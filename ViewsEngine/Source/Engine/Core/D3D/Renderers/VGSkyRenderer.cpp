#include "Engine/ECS/Components/Component.h"
#include "VGIRenderer.h"
#include "D3D/Shaders/Base/VGMaterial.h"
#include "D3D/Shaders/Shaders/VGShaderBase.h"

#include "VGSkyRenderer.h"

VGSkyRenderer::VGSkyRenderer() : VGIRenderer()
{
}

VGSkyRenderer::~VGSkyRenderer() = default;

void VGSkyRenderer::Render(ID3D12GraphicsCommandList* cmdList)
{
	if (!m_Mat || !m_Mesh) return;
	const auto shader = m_Mat->GetShader();
	
	shader->BeginDraw(cmdList);

	shader->Draw(cmdList, this);

	shader->EndDraw(cmdList);
}

void VGSkyRenderer::Update(float dt)
{
	if (!IsEnabled() || !m_Mat || !m_Mesh) return;

	if (transform->IsDirty())
		transform->UpdateParentedWorldMatrix();

	const auto shader = m_Mat->GetShader();
	auto objC = shader->GetNewObjectData();
	objC.World = *transform->GetTransposedParentedWorldMatrix();
	m_Mat->GetShader()->UpdateObjectCb(objC, m_ObjectCbIndex);
}



