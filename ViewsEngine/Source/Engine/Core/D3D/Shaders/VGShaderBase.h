#pragma once
#include <stack>

#include "D3D/Shaders/Base/VGUploadBuffer.h"

class VGIRenderer;

/*
struct ObjConstants
{
    ObjConstants() = default;
    DirectX::XMFLOAT4X4 World = Identity4X4();
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = Identity4X4();
    DirectX::XMFLOAT4X4 OrthoProj = Identity4X4();
    DirectX::XMFLOAT4X4 ViewProj = Identity4X4();

    DirectX::XMFLOAT4 LightColor = {1.0f, 0.0f, 1.0f, 1.0f};

    DirectX::XMFLOAT3 EyePosW = {0.0f, 0.0f, 0.0f};
    float TotalTime = 0.0f;

    DirectX::XMFLOAT3 LightDirection = {-1.0f, -1.0f, 0.0f};
    float DeltaTime = 0.0f;
};
 */

template <typename O = ObjConstants, typename P = PassConstants> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
class VGShader 
{
public:
    VGShader(ID3D12Device* device, const UINT cbvDescriptorSize, const std::wstring& filepath);
    ~VGShader() override;

public:
    void Init() override;
    void UnBind(const UINT index) override;
    VGShader* Bind() override;

    UINT GetCreatedIndex() override;
    UINT GetLastIndex() override;

    O GetNewObjectData() const override;
    void AddObjectCb() override;
    void UpdateObjectCb(void* data, UINT cbIndex) override;

    P GetNewPassData() const override;
    void CreatePassCb() override;
    void UpdatePassCb(void* data) const override;

protected:
    std::vector<VGUploadBuffer<O>*> m_ObjectCBs;
    std::stack<UINT> m_FreeIndices;
    VGUploadBuffer<P>* m_PassCb;
};

#pragma region Constructors and Destructors
template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
VGShader<O, P>::VGShader(ID3D12Device* device, const UINT cbvDescriptorSize, const std::wstring& filepath)
    : VGShader(device, cbvDescriptorSize, filepath)
{
    m_PassCb = nullptr;
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
VGShader<O, P>::~VGShader()
{
    for (auto& cb : m_ObjectCBs)
        DELPTR(cb)

    m_ObjectCBs.clear();
    DELPTR(m_PassCb)
}
#pragma endregion

#pragma region Init & Management
template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShader<O, P>::Init()
{
    VGShader::Init();
    CreatePassCb();
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShader<O, P>::UnBind(const UINT index)
{
    DELPTR(m_ObjectCBs[index])
    m_FreeIndices.push(index);
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
VGShader<O, P>* VGShader<O, P>::Bind()
{
    AddObjectCb();
    return this;
}
#pragma endregion

#pragma region Object Constant Buffer Management
template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
UINT VGShader<O, P>::GetCreatedIndex()
{
    return static_cast<UINT>(m_ObjectCBs.size()) - 1;
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
UINT VGShader<O, P>::GetLastIndex()
{
    return static_cast<UINT>(m_ObjectCBs.size());
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
O VGShader<O, P>::GetNewObjectData() const
{
    return O();
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShader<O, P>::AddObjectCb()
{
    auto ub = new VGUploadBuffer<O>(m_GeneralDevice, 1, true);
    m_ObjectCBs.push_back(ub);
    NULLPTR(ub)
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShader<O, P>::UpdateObjectCb(void* data, UINT cbIndex)
{
    if (cbIndex >= m_ObjectCBs.size()) AddObjectCb();

    m_ObjectCBs[cbIndex]->CopyData(0, &data);
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
P VGShader<O, P>::GetNewPassData() const
{
    return P();
}
#pragma endregion

#pragma region Pass Constant Buffer Management
template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShader<O, P>::CreatePassCb()
{
    m_PassCb = new VGUploadBuffer<P>(m_GeneralDevice, 1, true);
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShader<O, P>::UpdatePassCb(void* data) const
{
    m_PassCb->CopyData(0, &data);
}
#pragma endregion
