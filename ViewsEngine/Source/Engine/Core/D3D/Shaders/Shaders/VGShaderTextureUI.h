#pragma once

#include "VGShaderTexture.h"

class VGShaderTextureUI final : public VGShaderTexture
{
public:
    struct OffSetConstants
    {
        DirectX::XMFLOAT4X4 World = Identity4X4();
        float UVOffsetY = 0.0f;
    };

public:
    VGShaderTextureUI(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath);
    ~VGShaderTextureUI() override;

    UINT GetCreatedIndex() override { return static_cast<UINT>(m_OffSetCb.size()) - 1; }

    void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) override;
    void UpdateAsOffset(const DirectX::XMFLOAT4X4* itemWorldMatrix, UINT cbIndex, float offSetY);

protected:
    void AddObjectCb() override;

    /* We created a new object constant buffer to store the offset value of the UI element. */
    std::vector<VGUploadBuffer<OffSetConstants>*> m_OffSetCb;
};
