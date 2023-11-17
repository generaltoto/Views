#include "SkyBox.h"

SkyBox::SkyBox()
{
	m_Name = "SkyBox";

	auto mr = AddComponent<VGMeshRenderer>();
	mr->Init(MeshType::SPHERE, MaterialType::SKYBOX);
	mr->RegisterTexture(Resource::Load<VGCubemap>("Resources/Textures/cubemap.dds"));
	mr->SetClippable(false);
	

	transform->SetScale(500.f, 500.0f, 500.0f);
	transform->SetPosition(0.0f, 0.0f, 0.0f);

	mr = nullptr;
}

SkyBox::~SkyBox()
{
}
