#pragma once

#include "D3D/Shaders/VGMaterial.h"

class VGMaterialTextureUi : public VGMaterial
{
public:
    VGMaterialTextureUi();
    ~VGMaterialTextureUi() override;

    void Update() override;
    void Render() override;

private:
    XMFLOAT2 m_UvOffset;
    XMFLOAT2 m_Scale;
    
};
