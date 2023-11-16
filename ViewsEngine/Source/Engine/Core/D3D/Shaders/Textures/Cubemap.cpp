#include "Cubemap.h"

Cubemap::Cubemap(const std::string& name) : Texture(name)
{
	m_ResType = D3D12_SRV_DIMENSION_TEXTURECUBE;
}

Cubemap::~Cubemap() = default;



