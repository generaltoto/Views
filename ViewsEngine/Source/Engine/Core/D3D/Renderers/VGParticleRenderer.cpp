#include "D3D/Shaders/VGMaterial.h"
#include "D3D/Shaders/List/Particle/VGShaderParticle.h"

#include "VGParticleRenderer.h"

using namespace DirectX;

VGParticleRenderer::VGParticleRenderer() : VGIRenderer()
{
	srand(time(nullptr));

	m_Particles.resize(0);
}

VGParticleRenderer::~VGParticleRenderer()
{
	for (auto& p : m_Particles)
		DELPTR(p)

	m_Particles.clear();
}


void VGParticleRenderer::Init(const MeshType meshType, const MaterialType matType)
{
	VGIRenderer::Init(meshType, matType);
}

void VGParticleRenderer::Play() const
{
	for (UINT i = 0; i < m_ParticleCount; i++)
		m_Particles[i]->Awake();
}

void VGParticleRenderer::Pause() const
{
	for (UINT i = 0; i < m_ParticleCount; i++)
		m_Particles[i]->Sleep();
}

void VGParticleRenderer::Stop()
{
	for (UINT i = 0; i < MAX_PARTICLE_COUNT; i++)
		DELPTR(m_Particles[i])

	m_Particles.clear();
}

void VGParticleRenderer::AddParticles(const UINT count)
{
	m_ParticleCount = (m_ParticleCount + count > MAX_PARTICLE_COUNT) ? MAX_PARTICLE_COUNT : m_ParticleCount + count;
	Prepare();
}

UINT VGParticleRenderer::GetParticleCount() const
{
	return m_ParticleCount;
}


void VGParticleRenderer::Render(ID3D12GraphicsCommandList* cmdList)
{
	if (!IsEnabled() || !m_Mat || !m_Mesh) return;
	m_Mat->Render();
}

void VGParticleRenderer::Update(float dt)
{
	if (!IsEnabled() || !m_Mat || !m_Mesh) return;

	UpdateParticles(dt);
}


void VGParticleRenderer::Prepare()
{
	CreateMissingParticles();

	UpdateShaderBuffer();
}

void VGParticleRenderer::CreateMissingParticles()
{
	m_Particles.resize(m_ParticleCount, nullptr);
	
	for (UINT i = 0; i < m_ParticleCount; i++)
	{
		if (m_Particles[i] != nullptr) continue;

		CreateParticle();
	}
}

void VGParticleRenderer::CreateParticle()
{
	auto* p = new VGParticle();

	const float rLiftTime = rand() % 3 + 1.0f;

	const float randomDirX = ((static_cast<float>(rand() % 100) * 0.25f) - 0.5f) * 2.0f; // Get a random number between -1 and 1
	const float randomDirY = ((static_cast<float>(rand() % 100) * 0.25f) - 0.5f) * 2.0f; // ..
	const float randomDirZ = ((static_cast<float>(rand() % 100) * 0.25f) - 0.5f) * 2.0f; // ..
	const XMFLOAT3 rVel = { randomDirX, randomDirY, randomDirZ };

	const float randomRotX = ((static_cast<float>(rand() % 100) * 0.1f) - 5.0f) * 2.0f; // Get a random number between -10 and 10
	const float randomRotY = ((static_cast<float>(rand() % 100) * 0.1f) - 5.0f) * 2.0f; // ..
	const float randomRotZ = ((static_cast<float>(rand() % 100) * 0.1f) - 5.0f) * 2.0f; // ..
	const XMFLOAT3 rAngVel = { randomRotX, randomRotY, randomRotZ };

	const float randomScale = ((static_cast<float>(rand() % 100) * 0.01f) * 5) + 0.1f; // Get a random number between 0.1f and 0.35f
	p->Transform->SetScale(randomScale, randomScale, randomScale);

	p->Init(rLiftTime, rVel, rAngVel, transform->GetPosition());
	p->Awake();
	
	m_Particles.push_back(p);
}


void VGParticleRenderer::UpdateParticles(const float dt)
{
	for (UINT i = 0; i < m_ParticleCount; i++)
	{
		VGParticle* p = m_Particles[i];

		// Since some particles might be deleted, we need to check if the pointer is still valid
		if (p == nullptr) continue;

		// If the particle is dead, delete it
		if (!p->IsAlive() || !p->IsActive())
		{
			DELPTR(p)
			m_Particles.erase(m_Particles.begin() + i);
			m_ParticleCount--;
			if (i > 0)
				i--;
			continue;
		}

		// Update particle's lifetime and InstanceData AgeRatio
		p->CurrentLifeTime += dt;

		// Update position with particle's velocity
		p->Transform->Translate(p->Velocity.x * dt, p->Velocity.y * dt, p->Velocity.z * dt);

		// Update rotation with particle's angular velocity
		p->Transform->Rotate(p->AngularVelocity.x * dt, p->AngularVelocity.y * dt, p->AngularVelocity.z * dt);

		// Update InstanceData World matrix
		p->Transform->UpdateWorldMatrix();
	}

	UpdateShaderBuffer();
}

void VGParticleRenderer::UpdateShaderBuffer() const
{
	/*
	// Update the InstanceData buffer for each particle
	for (int i = 0; i < static_cast<int>(m_Particles.size()); i++)
	{
		auto objC = shader->GetNewObjectData();
		const auto particle = m_Particles[i];
			
		objC.World = *particle->Transform->GetWorldMatrix();
		objC.Color1 = m_Color1;
		objC.Color2 = m_Color2;
		objC.AgeRatio = (particle->LifeTime - particle->CurrentLifeTime) / particle->LifeTime;
			
		shader->UpdateParticleInstanceDataBuffer(i, &objC);
	}
	*/
}
