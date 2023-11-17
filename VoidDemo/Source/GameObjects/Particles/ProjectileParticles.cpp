#include "ProjectileParticles.h"

ProjectileParticles::ProjectileParticles()
{
	m_Name = "Particles";

	const auto pr = AddComponent<VGParticleRenderer>();
	pr->Init(MeshType::CUBE, MaterialType::PARTICLE);
}

ProjectileParticles::~ProjectileParticles()
{
}
