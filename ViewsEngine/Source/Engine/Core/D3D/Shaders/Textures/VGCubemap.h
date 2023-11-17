#pragma once

#include "VGTexture.h"

/*
Special texture type for cubemaps textures.
*/
class VGCubemap final : public VGTexture
{
public:
    explicit VGCubemap(const std::string& name);
    ~VGCubemap() override;
};
