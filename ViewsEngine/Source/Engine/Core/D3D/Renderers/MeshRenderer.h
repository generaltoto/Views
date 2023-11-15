#pragma once

#include "IRenderer.h"

/*
Classic mesh renderer component, used to render a mesh with a material
*/
class MeshRenderer final : public Component, public IRenderer
{
	friend class VECS_MeshRendererSystem;

public:
	MeshRenderer();
	~MeshRenderer() override;

private:
	void Render(ID3D12GraphicsCommandList* cmdList) override;
	void Update(float dt) override;

public:
	void SetClippable(const bool value) { m_IsClippable = value; }
	[[nodiscard]] bool IsClippable() const { return m_IsClippable; }

private:
	// Should this mesh be clipped by the frustum or not
	bool m_IsClippable;

};
