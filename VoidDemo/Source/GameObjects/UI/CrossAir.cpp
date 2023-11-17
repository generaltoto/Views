

#include "CrossAir.h"

CrossAir::CrossAir()
{
	auto uir = AddComponent<VGUiRenderer>();
	uir->Init(SQUARE_UI_SIMPLE, TEXTURE_UI);
	uir->RegisterTexture(Resource::Load<VGTexture>("Resources/Textures/crossAir1.dds"));
}

CrossAir::~CrossAir()
{
}
