#include "Wing.h"

Wing::Wing()
{
	auto mr = AddComponent<VGMeshRenderer>();
	mr->Init(CUBE, SIMPLE);
	mr->SetClippable(false);

	transform->SetScale(0.5f, 0.5f, 3);
}

Wing::~Wing()
{
}
