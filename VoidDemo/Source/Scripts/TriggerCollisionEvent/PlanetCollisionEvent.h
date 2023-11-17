#pragma once
#include "Scripts/Asteroids/PlanetLife.h"

class PlanetCollisionEvent : public TriggerCollisionEvent
{
	public:

	PlanetCollisionEvent();
	~PlanetCollisionEvent() override;

	void OnAddedComponent() override;
	void OnTriggerEnter(Collider* other) override;

	void GetParticleRenderer();

private:

	VGParticleRenderer * m_ParticleRenderer = nullptr;
	PlanetLife* m_PlanetLife = nullptr;

};

