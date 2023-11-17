#include "VGTexture2D.h"

VGTexture2D::VGTexture2D(const std::string& name) : VGTexture(name)
{
    m_ResType = D3D12_SRV_DIMENSION_TEXTURE2D;
}

VGTexture2D::~VGTexture2D() = default;
