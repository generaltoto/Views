#pragma once

#include <set>
#include "TypeDef.h"

class System {

public:
	virtual ~System() = default;

	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void Update(float delta) = 0;
	virtual void Render() = 0;

public:
	std::set<InstanceID> RegisteredEntities;
};
