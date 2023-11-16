#pragma once

#include "Texture.h"

class Texture2D final : public Texture
{
public:
    explicit Texture2D(const std::string& name);
    ~Texture2D() override;
};
