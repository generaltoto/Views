#include "VGCubemap.h"

VGCubemap::VGCubemap(const std::string& name) : VGTexture(name)
{
	m_ResType = D3D12_SRV_DIMENSION_TEXTURECUBE;
}

VGCubemap::~VGCubemap() = default;



