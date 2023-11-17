#include "Engine/ECS/Components/Component.h"
#include "VGIRenderer.h"
#include "D3D/Shaders/VGMaterial.h"
#include "D3D/Shaders/VGShaderBase.h"

#include "VGSkyRenderer.h"

VGSkyRenderer::VGSkyRenderer() : VGIRenderer()
{
}

VGSkyRenderer::~VGSkyRenderer() = default;

void VGSkyRenderer::Render(ID3D12GraphicsCommandList* cmdList)
{
	if (!Mat || !Mesh) return;
	const auto shader = Mat->GetShader();
	
	shader->BeginDraw(cmdList);

	shader->Draw(cmdList, this);

	shader->EndDraw(cmdList);
}

void VGSkyRenderer::Update(float dt)
{
	if (!IsEnabled() || !Mat || !Mesh) return;

	if (transform->IsDirty())
		transform->UpdateParentedWorldMatrix();

	Mat->GetShader()->UpdateObjectCb(transform->GetTransposedParentedWorldMatrix(), ObjectCbIndex);
}



