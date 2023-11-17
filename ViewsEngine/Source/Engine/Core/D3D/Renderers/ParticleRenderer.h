#pragma once

#include "D3D/Geometry/VGParticle.h"

#define MAX_PARTICLE_COUNT 5000

/*
Particle Renderer is a component that renders particles.
For now, it is used as a general renderer for all particles.
*/
class ParticleRenderer final : public Component, public IRenderer
{
	friend class VECS_ParticleRendererSystem;
public:
	ParticleRenderer();
	~ParticleRenderer() override;

	void Play() const;
	void Pause() const;
	void Stop();

	void Init(MeshType meshType, MaterialType matType) override;

	void AddParticles(UINT count);
	[[nodiscard]] UINT GetParticleCount() const;

	DirectX::XMFLOAT4 GetColor1() const { return m_color1; }
	DirectX::XMFLOAT4 GetColor2() const { return m_color2; }

private:
	std::vector<VGParticle*> m_particles{};
	std::vector<InstanceData> m_particleInstanceData{};

	UINT m_particleCount = 0;

	DirectX::XMFLOAT4 m_color1;
	DirectX::XMFLOAT4 m_color2;

	void Render(ID3D12GraphicsCommandList* cmdList) override;
	void Update(float dt) override;

	void Prepare();
	void CreateMissingParticles();
	void CreateParticle();

	void UpdateParticles(float dt);
	void UpdateShaderBuffer() const;
};