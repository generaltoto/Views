#include "Texture2D.h"

Texture2D::Texture2D(const std::string& name) : Texture(name)
{
    m_ResType = D3D12_SRV_DIMENSION_TEXTURE2D;
}

Texture2D::~Texture2D() = default;
