#pragma once

#include "VGShaderBase.h"

class VGShaderParticle final : public VGShaderBase
{
    public:
    VGShaderParticle(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath);
    ~VGShaderParticle() override;

    void Init() override;
    void CreatePsoAndRootSignature(DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) override;

    void BeginDraw(ID3D12GraphicsCommandList* cmdList) override;
    void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) override;
    void EndDraw(ID3D12GraphicsCommandList* cmdList) override;

    void UpdateParticleInstanceDataBuffer(int startIndex, const void* data) const;
    private:
    void DrawAsParticle(ID3D12GraphicsCommandList* cmdList, const VGParticleRenderer* drawnMeshR);

    /*
    To render particles with a minimum amount of draw calls, we need to use instancing.
    Instancing allows us to draw multiple instances of the same mesh with only one draw call.
    
    To do so, we need to create a buffer that will store the data of each instance.
    We can then give the buffer to the shader that will catch it in a StructuredBuffer. We also pass the instanceID to the shader via the vs_main method.
    See Texture_UI.hlsl and chapter 16.1 for more information.
    */
    VGUploadBuffer<InstanceData>* m_ParticleInstanceDataBuffer;
};
