#pragma once

#include "D3D/Geometry/VGParticle.h"
#include "VGIRenderer.h"

struct InstanceData
{
	DirectX::XMFLOAT4X4 World = Identity4X4();
	DirectX::XMFLOAT4 Color1 = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT4 Color2 = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	float AgeRatio = 0.0f;
};

#define MAX_PARTICLE_COUNT 5000

/*
Particle Renderer is a component that renders particles.
For now, it is used as a general renderer for all particles.
*/
class VGParticleRenderer final : public Component, public VGIRenderer
{
	friend class VECS_ParticleRendererSystem;
public:
	VGParticleRenderer();
	~VGParticleRenderer() override;

	void Play() const;
	void Pause() const;
	void Stop();

	void Init(MeshType meshType, MaterialType matType) override;

	void AddParticles(UINT count);
	[[nodiscard]] UINT GetParticleCount() const;

	DirectX::XMFLOAT4 GetColor1() const { return m_Color1; }
	DirectX::XMFLOAT4 GetColor2() const { return m_Color2; }

private:
	std::vector<VGParticle*> m_Particles{};

	UINT m_ParticleCount = 0;

	DirectX::XMFLOAT4 m_Color1;
	DirectX::XMFLOAT4 m_Color2;

	void Render(ID3D12GraphicsCommandList* cmdList) override;
	void Update(float dt) override;

	void Prepare();
	void CreateMissingParticles();
	void CreateParticle();

	void UpdateParticles(float dt);
	void UpdateShaderBuffer() const;
};