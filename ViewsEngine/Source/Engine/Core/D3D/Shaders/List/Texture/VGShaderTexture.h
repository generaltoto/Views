#pragma once

#include "D3D/Shaders/VGShaderBase.h"


class VGShaderTexture : public VGShaderBase<>
{
public:
    VGShaderTexture(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath);
    ~VGShaderTexture() override;

    void Init() override;
    void CreatePsoAndRootSignature(DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) override;

    void BeginDraw(ID3D12GraphicsCommandList* cmdList) override;
    void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) override;
    void EndDraw(ID3D12GraphicsCommandList* cmdList) override;
};


