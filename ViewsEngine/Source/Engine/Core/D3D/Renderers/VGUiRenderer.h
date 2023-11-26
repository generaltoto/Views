#pragma once 
#include "Engine/ECS/Components/Component.h"
#include "VGIRenderer.h"

/*
UIRenderer is a component that is used to render UI elements such as score, health, etc.
Be careful to use shaders that supports UI rendering, such as ShaderTextureOffset.
*/
class VGUiRenderer final : public VGIRenderer
{
	friend class VECS_UiRenderSystem;

public:
	VGUiRenderer();
	~VGUiRenderer() override;

	[[nodiscard]] float GetUVOffsetY() const { return m_uvOffsetY; }
	void SetOffsetY(float offsetY);
	void AddOffsetY(float offsetY);

private:
	void Render(ID3D12GraphicsCommandList* cmdList) override;
	void Update(float dt) override;
	
	float m_uvOffsetY;
};
