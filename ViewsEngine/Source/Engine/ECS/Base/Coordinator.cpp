
#include "Coordinator.h"

#include "D3D/Renderers/MeshRenderer.h"
#include "D3D/Renderers/ParticleRenderer.h"
#include "D3D/Renderers/SkyRenderer.h"
#include "D3D/Renderers/UIRenderer.h"
#include "Engine/ECS/Components/Collision/Collider.h"
#include "Engine/ECS/Components/Collision/RigidBody.h"

#include "Engine/ECS/Systems/VECS_UiRenderSystem.h"
#include "Engine/ECS/Systems/VECS_MeshRendererSystem.h"
#include "Engine/ECS/Systems/VECS_ParticleRendererSystem.h"
#include "Engine/ECS/Systems/VECS_SkyRendererSystem.h"



Coordinator* Coordinator::m_Instance = nullptr;

Coordinator::Coordinator()
{
}

Coordinator::~Coordinator()
{
	m_EntityManager.reset();
	m_SystemManager.reset();
	m_ComponentManager.reset();
}

void Coordinator::Init()
{
	// Create pointers to each manager
	m_ComponentManager = std::make_unique<ComponentManager>();
	m_EntityManager = std::make_unique<EntityManager>();
	m_SystemManager = std::make_unique<SystemManager>();

	RegisterComponents();
	RegisterSystems();
}

void Coordinator::RegisterComponents()
{
	// TODO: Automatize this

	RegisterComponent<Transform>();
	RegisterComponent<MeshRenderer>();
	RegisterComponent<Rigidbody>();
	RegisterComponent<SphereCollider>();
	RegisterComponent<CameraComponent>();
	RegisterComponent<ParticleRenderer>();
	RegisterComponent<SkyRenderer>();
	RegisterComponent<UIRenderer>();
}

void Coordinator::RegisterSystems()
{
	RegisterSystem<VECS_UiRenderSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<UIRenderer>());

		SetSystemSignature<VECS_UiRenderSystem>(signature);
	}

	RegisterSystem<VECS_MeshRendererSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<MeshRenderer>());

		SetSystemSignature<VECS_MeshRendererSystem>(signature);
	}

	RegisterSystem<VECS_ParticleRendererSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<ParticleRenderer>());

		SetSystemSignature<VECS_ParticleRendererSystem>(signature);
	}

	RegisterSystem<VECS_SkyRendererSystem>();
	{
		Signature signature;
		signature.set(GetComponentType<SkyRenderer>());

		SetSystemSignature<VECS_SkyRendererSystem>(signature);
	}
}

GameObject* Coordinator::GetEntityByName(const std::string& name) const
{
	return m_EntityManager->GetEntityByName(name);
}


Coordinator* Coordinator::GetInstance()
{
	if (!m_Instance)
	{
		m_Instance = new Coordinator();
	}

	return m_Instance;
}

InstanceID Coordinator::GetNewInstanceID() const
{
	return m_EntityManager->GetNewInstanceID();
}

void Coordinator::RegisterGameObject(GameObject* go) const
{
	return m_EntityManager->RegisterGameObject(go);
}

void Coordinator::AwakeComponents() const
{
	m_ComponentManager->AwakeAllComponents();
}

void Coordinator::StartComponents() const
{
	m_ComponentManager->StartAllComponents();
}

void Coordinator::UpdateComponents() const
{
	m_ComponentManager->UpdateAllComponents();
}

void Coordinator::LateUpdateComponents() const
{
	m_ComponentManager->LateUpdateAllComponents();
}

void Coordinator::FixedUpdateComponents() const
{
	m_ComponentManager->FixedUpdateAllComponents();
}

void Coordinator::CleanUp() 
{
	DestroyRegisteredEntities();
	m_ComponentManager->CleanEverything();
	m_EntityManager->CleanEverything();
}

void Coordinator::RegisterCustomComponent(CustomComponent* customComponent) const
{
	m_ComponentManager->RegisterCustomComponent(customComponent);
}

void Coordinator::UnregisterCustomComponent(CustomComponent* customComponent) const
{
	m_ComponentManager->UnregisterCustomComponent(customComponent);
}

void Coordinator::SetEntityToBeDestroyed(const InstanceID entity)
{
	m_EntitiesToDestroy.push_back(entity);
}

void Coordinator::DestroyRegisteredEntities()
{
	if (m_EntitiesToDestroy.empty()) return;

	for (const auto entity : m_EntitiesToDestroy)
	{
		DestroyEntity(entity);
	}

	m_EntitiesToDestroy.clear();
}

void Coordinator::UpdateSystems(const float dt) const
{
	m_SystemManager->UpdateAllSystems(dt);
}

void Coordinator::RenderSystems() const
{
	m_SystemManager->RenderAllSystems();
}

void Coordinator::DestroyEntity(const InstanceID entity) const
{
	m_EntityManager->DestroyEntity(entity);
	m_SystemManager->EntityDestroyed(entity);
	m_ComponentManager->EntityDestroyed(entity);
}
