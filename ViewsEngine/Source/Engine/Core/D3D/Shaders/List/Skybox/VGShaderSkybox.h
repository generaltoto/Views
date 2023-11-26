#pragma once

#include "D3D/Shaders/List/Texture/VGShaderTexture.h"

class VGShaderSkybox final : public VGShaderTexture
{
public:
    VGShaderSkybox(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath);
    ~VGShaderSkybox() override;

    void CreatePsoAndRootSignature(DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) override;
};
