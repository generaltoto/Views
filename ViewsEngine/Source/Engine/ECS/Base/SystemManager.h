#pragma once

#include "System.h"

#include "../Core/DebugUtils.h"
#include <memory>
#include <unordered_map>
#include <cassert>


class SystemManager
{
public:

	SystemManager();
	~SystemManager();

	void UpdateAllSystems(float dt);

public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		assert(m_Systems.find(typeName) == m_Systems.end() && "Registering system more than once.");

		DEBUG_LOG("Registered " << typeName << " to system manager");

		// Create a pointer to the system and return it so it can be used externally
		auto system = std::make_shared<T>();
		m_Systems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void SetSignature(Signature signature)
	{
		const char* typeName = typeid(T).name();

		assert(m_Systems.contains(typeName) && "System used before registered.");

		// Set the signature for this system
		m_Signatures.insert({ typeName, signature });
	}

public:

	// Erase a destroyed entity from all system lists
	void EntityDestroyed(InstanceID entity);

	void EntitySignatureChanged(InstanceID entity, Signature entitySignature);

private:

	// Map from system type string pointer to a signature
	std::unordered_map<const char*, Signature> m_Signatures;

	// Map from system type string pointer to a system pointer
	std::unordered_map<const char*, std::shared_ptr<System>> m_Systems;
};