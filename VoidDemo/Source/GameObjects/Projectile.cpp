#include "BlankProject.h"
#include "Projectile.h"

#include "Scripts/ProjectileBehavior.h"

Projectile::Projectile()
{
	m_Name = "Projectile";

	AddComponent<MeshRenderer>()->Init(SPHERE, SIMPLE);
	AddComponent<Rigidbody>();
	AddComponent<SphereCollider>();

	AddComponent<ProjectileBehavior>();

	m_CategoryBitmask.SetLayer(LayerID::PROJECTILE);
	m_CollisionBitmask.SetLayer(LayerID::ASTEROID);
	m_CollisionBitmask.AddLayer(LayerID::OBSTACLE);

	transform->SetScale(0.15f, 0.15f, 0.15f);

}

Projectile::~Projectile()
{
}
