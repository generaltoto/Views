

#include "PlanetCollisionEvent.h"


void PlanetCollisionEvent::OnAddedComponent()
{
	TriggerCollisionEvent::OnAddedComponent();
	
	GetParticleRenderer();
}

void PlanetCollisionEvent::OnTriggerEnter(Collider* other)
{
	if (other->gameObject->m_CategoryBitmask.GetLayer() == PROJECTILE)
	{
		if (m_ParticleRenderer == nullptr || !m_ParticleRenderer->transform) return;
		
		m_ParticleRenderer->transform->SetPosition(other->gameObject->transform->GetWorldPosition());
		m_ParticleRenderer->AddParticles(10);

		other->gameObject->Destroy();
	}
}

void PlanetCollisionEvent::GetParticleRenderer()
{
	//m_ParticleRenderer = GameObject::Find("Particles")->GetComponent<ParticleRenderer>();
}

PlanetCollisionEvent::PlanetCollisionEvent()
{
}

PlanetCollisionEvent::~PlanetCollisionEvent()
{
	m_ParticleRenderer = nullptr;
}