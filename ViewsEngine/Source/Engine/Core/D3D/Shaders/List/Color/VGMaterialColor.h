#pragma once

#include "D3D/Shaders/VGMaterial.h"

class VGMaterialColor final : public VGMaterial
{
public:
    VGMaterialColor();
    ~VGMaterialColor() override;

    XMFLOAT4& GetColor() { return m_Color; }
    
    void Update() override;
    void Render() override;

private:
    XMFLOAT4 m_Color;
};
