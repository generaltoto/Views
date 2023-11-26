#pragma once

#include "D3D/Shaders/VGMaterial.h"

class VGMaterialParticle final : public VGMaterial
{
public:
    VGMaterialParticle();
    ~VGMaterialParticle() override;

    void Update() override;
    void Render() override;

private:
    XMFLOAT4 m_Color1;
    XMFLOAT4 m_Color2;

    float m_AgeRatio;
};
