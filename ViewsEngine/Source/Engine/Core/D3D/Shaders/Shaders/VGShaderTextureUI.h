#pragma once

#include "VGShaderTexture.h"

struct UIObjConstants : ObjConstants
{
    XMFLOAT2 Offset = {0.0f, 0.0f};
    XMFLOAT2 Scale = {1.0f, 1.0f};
};

class VGShaderTextureUI final : public VGShaderBase<UIObjConstants>
{
public:
    VGShaderTextureUI(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath);
    ~VGShaderTextureUI() override;

    void Init() override;
    void CreatePsoAndRootSignature(DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) override;

    void BeginDraw(ID3D12GraphicsCommandList* cmdList) override;
    void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) override;
    void EndDraw(ID3D12GraphicsCommandList* cmdList) override;
};
