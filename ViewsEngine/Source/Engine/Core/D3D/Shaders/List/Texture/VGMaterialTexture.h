#pragma once

#include "D3D/Shaders/VGMaterial.h"

class VGMaterialTexture : public VGMaterial
{
public:
    VGMaterialTexture();
    ~VGMaterialTexture() override;

    void Update() override;
    void Render() override;
};
