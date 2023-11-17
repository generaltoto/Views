#include "Planet.h"

#include "Scripts/Asteroids/PlanetLife.h"
#include "Scripts/TriggerCollisionEvent/PlanetCollisionEvent.h"

Planet::Planet()
{
	m_Name = "Planet";

	auto rb = AddComponent<Rigidbody>();
	rb->SetMass(100.f);
	rb->SetBodyType(BodyType::Static);

	auto mesh = AddComponent<VGMeshRenderer>();
	mesh->Init(SPHERE, TEXTURE);
	mesh->RegisterTexture(Resource::Load<VGTexture>("Resources/Textures/mars.dds"));

	transform->SetScale(50);

	auto sc = AddComponent<SphereCollider>();
	sc->SetRadius(50);

	AddComponent<PlanetLife>();

	m_CategoryBitmask.SetLayer(LayerID::OBSTACLE);
	m_CollisionBitmask.SetLayer(LayerID::PLAYER);
	m_CollisionBitmask.AddLayer(LayerID::ASTEROID);
	m_CollisionBitmask.AddLayer(LayerID::PROJECTILE);

	AddComponent<PlanetCollisionEvent>();

	mesh = nullptr;
}

Planet::~Planet()
{
}
