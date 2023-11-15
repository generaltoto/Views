#include "Engine/ECS/Components/Component.h"
#include "IRenderer.h"
#include "D3D/Shaders/Material.h"
#include "D3D/Shaders/ShaderBase.h"

#include "SkyRenderer.h"

SkyRenderer::SkyRenderer() : IRenderer()
{
}

SkyRenderer::~SkyRenderer() = default;

void SkyRenderer::Render(ID3D12GraphicsCommandList* cmdList)
{
	if (!Mat || !Mesh) return;
	const auto shader = Mat->GetShader();
	
	shader->BeginDraw(cmdList);

	shader->Draw(cmdList, this);

	shader->EndDraw(cmdList);
}

void SkyRenderer::Update(float dt)
{
	if (!IsEnabled() || !Mat || !Mesh) return;

	if (transform->IsDirty())
		transform->UpdateParentedWorldMatrix();

	Mat->GetShader()->UpdateObjectCB(transform->GetTransposedParentedWorldMatrix(), ObjectCBIndex);
}



