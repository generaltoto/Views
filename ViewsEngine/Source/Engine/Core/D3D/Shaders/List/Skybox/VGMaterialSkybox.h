#pragma once

#include "D3D/Shaders/VGMaterial.h"

class VGMaterialSkybox final : public VGMaterial
{
public:
    VGMaterialSkybox();
    ~VGMaterialSkybox() override;

    void Update() override;
    void Render() override;
};
