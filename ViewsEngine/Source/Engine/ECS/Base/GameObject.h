#pragma once

#include "Object.h"
#include "Engine/ECS/Base/Coordinator.h"
#include "Engine/Core/DebugUtils.h"
#include "Engine/Managers/LayerManager.h"


class GameObject : public Object
{
public:
	GameObject();
	GameObject(const std::string &name);


	~GameObject() override;

	static GameObject* Instantiate()
	{
		return new GameObject();
	}

	template <class GameObject>
	static GameObject* Instantiate() 
	{
		return new GameObject();
	}

	template <class GameObject>
	static GameObject* Instantiate(const std::string& name)
	{
		auto go = new GameObject();
		go->SetName(name);

		return go;
	}
	//static Object Instantiate(GameObject original, Transform parent);
	//static Object Instantiate(Object original, Transform parent, bool instantiateInWorldSpace);

	/*
	template <class GameObject>
	static GameObject* Instantiate(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, Transform parent)
	{
		auto go = new GameObject(original);
		go->transform->SetPosition(position);
		go->transform->Rotate(rotation);

		//TODO: Set parent

		return go;
	}*/

	template<class T>
	T* AddComponent()
	{
		auto component = new T();
		component->gameObject = this;
		component->transform = transform;
		component->SetEnabled(true);

		//DEBUG_LOG("Adding component: " + std::string(typeid(T).name()) + " to " + m_Name + " entity");

		Coordinator::GetInstance()->AddComponent<T>(GetInstanceID(), component);

		component->OnAddedComponent();

		return component;
	}

	template <class Component>
	Component* AddComponent(Component *component)
	{
		component->gameObject = this;
		component->transform = transform;
		component->SetEnabled(true);

		//DEBUG_LOG("Adding component: " + std::string(typeid(Component).name()) + " to " + m_Name + " entity");

		Coordinator::GetInstance()->AddComponent<Component>(GetInstanceID(), component);

		component->OnAddedComponent();

		return component;
	}

	template <class T>
	T* GetComponent()
	{
		return Coordinator::GetInstance()->GetComponent<T>(GetInstanceID());
	}

	template <class T>
	bool HasComponent() const
	{
		return Coordinator::GetInstance()->HasComponent<T>(GetInstanceID());
	}

	static GameObject* Find(const std::string& name);

	bool IsActive() const { return m_IsActive; }
	void SetActive(bool value) { m_IsActive = value; }

public:

	Transform *transform;
	//Add bitset for collision bitmask

	// Set bit for the category of the object
	PhysicLayer m_CategoryBitmask;

	// Decides which categories this object collides with
	PhysicLayer m_CollisionBitmask;

		 
private:

	bool m_IsActive;

};
