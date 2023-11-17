#include "VGShaderTextureUI.h"

#include "D3D/Geometry/VGMesh.h"
#include "D3D/Base/VGHandler.h"

VGShaderTextureUI::VGShaderTextureUI(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, const UINT cbvDescriptorSize, std::wstring& filepath)
    : VGShaderTexture(device, cbvHeap, cbvDescriptorSize, filepath)
{
}

VGShaderTextureUI::~VGShaderTextureUI()
{
    for (auto& cb : m_OffSetCb)
        DELPTR(cb)
}

void VGShaderTextureUI::Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR)
{
    // UINT are not supposed to be negative, it will only be negative if it is not initialized (debug only).
    // ReSharper disable once CppUnsignedZeroComparison
    assert(drawnMeshR->GetTexture(0)->GetHeapIndex() >= 0);

    if (drawnMeshR->ObjectCbIndex >= m_OffSetCb.size())
        AddObjectCb();

    assert(drawnMeshR->ObjectCbIndex <= m_OffSetCb.size());

    cmdList->IASetVertexBuffers(0, 1, &drawnMeshR->Mesh->VertexBufferView());
    cmdList->IASetIndexBuffer(&drawnMeshR->Mesh->IndexBufferView());

    ID3D12DescriptorHeap* cbvSrvHeap = nullptr;
    UINT heapSize = I(VGHandler)->GetCbvHeap(&cbvSrvHeap);
    auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
    cbvHandle.Offset(drawnMeshR->GetTexture(0)->GetHeapIndex(), m_CbvDescriptorSize);

    cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);
    cmdList->SetGraphicsRootConstantBufferView(1, m_OffSetCb[drawnMeshR->ObjectCbIndex]->GetResource()->GetGPUVirtualAddress());


    cmdList->DrawIndexedInstanced(drawnMeshR->Mesh->GetIndexCount(), 1, 0, 0, 0);
}

void VGShaderTextureUI::AddObjectCb()
{
    auto ub = new VGUploadBuffer<OffSetConstants>(m_GeneralDevice, 1, true);
    m_OffSetCb.push_back(ub);
    NULLPTR(ub)

    VGShaderBase::AddObjectCb();
}

void VGShaderTextureUI::UpdateAsOffset(const DirectX::XMFLOAT4X4* itemWorldMatrix, const UINT cbIndex, const float offSetY)
{
    if (cbIndex >= m_OffSetCb.size())
        AddObjectCb();

    OffSetConstants offSetConstants;
    offSetConstants.World = *itemWorldMatrix;
    offSetConstants.UVOffsetY = offSetY;
    m_OffSetCb[cbIndex]->CopyData(0, &offSetConstants);
}