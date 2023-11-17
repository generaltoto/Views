

#include "Scripts/UI/DigitBehavior.h"

#include "Digit.h"

Digit::Digit()
{
	auto meshRenderer = AddComponent<VGUiRenderer>();
	meshRenderer->Init(MeshType::SQUARE_UI_OFFSET_Y, MaterialType::TEXTURE_UI);
	meshRenderer->RegisterTexture(Resource::Load<VGTexture>("Resources/Textures/number.dds"));

	m_digitBehavior = AddComponent<DigitBehavior>();

	meshRenderer = nullptr;
}

Digit::~Digit()
{
	m_digitBehavior = nullptr;
}
