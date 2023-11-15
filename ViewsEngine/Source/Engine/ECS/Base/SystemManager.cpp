
#include "SystemManager.h"

#include <ranges>

SystemManager::SystemManager() : m_Signatures(), m_Systems()
{
}

SystemManager::~SystemManager()
{
	m_Systems.clear();
	m_Signatures.clear();
}

void SystemManager::UpdateAllSystems(const float dt) const
{
	for (const auto& system : m_Systems | std::views::values)
	{
		system->Update(dt);
	}
}

void SystemManager::RenderAllSystems()
{
	for (const auto& system : m_Systems | std::views::values)
	{
		system->Render();
	}
}


void SystemManager::EntityDestroyed(const InstanceID entity) const
{
	for (const auto& system : m_Systems | std::views::values)
	{
		system->RegisteredEntities.erase(entity);
	}
}

void SystemManager::EntitySignatureChanged(const InstanceID entity, const Signature entitySignature)
{
	// Notify each system that an entity's signature changed
	for (const auto& [type, system] : m_Systems)
	{
		// InstanceID signature matches system signature - insert into set
		if (auto const& systemSignature = m_Signatures[type]; (entitySignature & systemSignature) == systemSignature)
		{
			system->RegisteredEntities.insert(entity);
		}
		// InstanceID signature does not match system signature - erase from set
		else
		{
			system->RegisteredEntities.erase(entity);
		}
	}
}