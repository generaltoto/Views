
#include "ComponentManager.h"

#include "Engine/Engine.h"


ComponentManager::ComponentManager()
	: m_NextComponentType(0)
{
	m_ComponentArrays = {};
	m_ComponentTypes = {};
}

ComponentManager::~ComponentManager()
{
	CleanEverything();
}

void ComponentManager::AwakeAllComponents()
{
	const int size = m_CustomComponents.size();

	for (int i = 0; i < size; i++) 
	{
		auto const customComponent = m_CustomComponents[i];
		customComponent->Awake();
		customComponent->SetInitialized();
	}
}

void ComponentManager::StartAllComponents()
{
	for (auto const customComponent : m_CustomComponents)
	{
		if (!customComponent->gameObject->IsActive() || !customComponent->IsEnabled()) continue;

		customComponent->Start();
	}
}

void ComponentManager::UpdateAllComponents()
{
	const int size = m_CustomComponents.size();
	for (int i = 0; i < size; i++)
	{
		auto const customComponent = m_CustomComponents[i];

		if (!customComponent->gameObject->IsActive() || !customComponent->IsEnabled() || !customComponent->IsInitialized()) continue;

		customComponent->Update();
	}
}

void ComponentManager::FixedUpdateAllComponents()
{
	for (auto const customComponent : m_CustomComponents)
	{
		if (!customComponent->gameObject->IsActive() || !customComponent->IsEnabled()) continue;

		customComponent->FixedUpdate();
	}
}

void ComponentManager::LateUpdateAllComponents()
{
	for (auto const customComponent : m_CustomComponents)
	{
		if (!customComponent->gameObject->IsActive() || !customComponent->IsEnabled()) continue;

		customComponent->LateUpdate();
	}
}

void ComponentManager::CleanEverything() 
{
	for (auto const& pair : m_ComponentArrays)
	{
		auto const& component = pair.second;

		component->CleanUp();
	}

	m_CustomComponents.clear();
}

void ComponentManager::RegisterCustomComponent(CustomComponent* customComponent)
{
	if (Engine::GetInstance()->GetEngineState() == Engine::RUNTIME)
	{
		customComponent->Awake();
		customComponent->Start();
		customComponent->SetInitialized();
	}

	m_CustomComponents.push_back(customComponent);
}

void ComponentManager::UnregisterCustomComponent(CustomComponent* customComponent)
{
	std::erase(m_CustomComponents, customComponent);
}

void ComponentManager::EntityDestroyed(InstanceID entity)
{
	for (auto const& pair : m_ComponentArrays)
	{
		auto const& component = pair.second;

		component->EntityDestroyed(entity);
	}
}

