#pragma once

#include "VGTexture.h"

class VGTexture2D final : public VGTexture
{
public:
    explicit VGTexture2D(const std::string& name);
    ~VGTexture2D() override;
};
