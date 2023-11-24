#pragma once
#include <stack>

#include "D3D/Base/VGUtils.h"
#include "D3D/Shaders/Base/VGUploadBuffer.h"

class VGIRenderer;

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

// This class servers as a base for all shaders. It it used for creating shader list without the VGShaderBase template.
class VGShader
{
public:
    VGShader(ID3D12Device* device, const UINT cbvDescriptorSize, std::wstring filepath);
    virtual ~VGShader();

    virtual void Init();
    virtual void UnBind(const UINT index) = 0;
    virtual VGShader* Bind() = 0;

    virtual void CreatePsoAndRootSignature(DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) = 0;

    [[nodiscard]] virtual ObjConstants GetNewObjectData() const = 0;
    virtual void AddObjectCb() = 0;
    virtual void UpdateObjectCb(void* data, UINT cbIndex) = 0;

    virtual UINT GetCreatedIndex() = 0;
    virtual UINT GetLastIndex() = 0;

    [[nodiscard]] virtual PassConstants GetNewPassData() const = 0;
    virtual void CreatePassCb() = 0;
    virtual void UpdatePassCb(void* data) const = 0;

    virtual void BeginDraw(ID3D12GraphicsCommandList* cmdList) = 0;
    virtual void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) = 0;
    virtual void EndDraw(ID3D12GraphicsCommandList* cmdList) = 0;

protected:
    std::wstring m_Filepath;

    std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

    ID3D12RootSignature* m_RootSignature;

    ID3D10Blob* m_VsByteCode;
    ID3D10Blob* m_PsByteCode;

    ID3D12PipelineState* m_PipelineState;

    ID3D12Device* m_GeneralDevice;
    UINT m_CbvDescriptorSize;

protected:
    void SetInputLayout();

    static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> GetStaticSamplers();

    void CompileShader(const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, ID3DBlob** uploader) const;
};

template <typename O = ObjConstants, typename P = PassConstants> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
class VGShaderBase : public VGShader
{
public:
    VGShaderBase(ID3D12Device* device, const UINT cbvDescriptorSize, const std::wstring& filepath);
    ~VGShaderBase() override;

public:
    void Init() override;
    void UnBind(const UINT index) override;
    VGShaderBase* Bind() override;

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
VGShaderBase<O, P>::VGShaderBase(ID3D12Device* device, const UINT cbvDescriptorSize, const std::wstring& filepath)
    : VGShader(device, cbvDescriptorSize, filepath)
{
    m_PassCb = nullptr;
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
VGShaderBase<O, P>::~VGShaderBase()
{
    for (auto& cb : m_ObjectCBs)
        DELPTR(cb)

    m_ObjectCBs.clear();
    DELPTR(m_PassCb)
}
#pragma endregion

#pragma region Init & Management
template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::Init()
{
    VGShader::Init();
    CreatePassCb();
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::UnBind(const UINT index)
{
    DELPTR(m_ObjectCBs[index])
    m_FreeIndices.push(index);
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
VGShaderBase<O, P>* VGShaderBase<O, P>::Bind()
{
    AddObjectCb();
    return this;
}
#pragma endregion

#pragma region Object Constant Buffer Management
template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
UINT VGShaderBase<O, P>::GetCreatedIndex()
{
    return static_cast<UINT>(m_ObjectCBs.size()) - 1;
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
UINT VGShaderBase<O, P>::GetLastIndex()
{
    return static_cast<UINT>(m_ObjectCBs.size());
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
O VGShaderBase<O, P>::GetNewObjectData() const
{
    return O();
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::AddObjectCb()
{
    auto ub = new VGUploadBuffer<O>(m_GeneralDevice, 1, true);
    m_ObjectCBs.push_back(ub);
    NULLPTR(ub)
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::UpdateObjectCb(void* data, UINT cbIndex)
{
    if (cbIndex >= m_ObjectCBs.size()) AddObjectCb();

    m_ObjectCBs[cbIndex]->CopyData(0, &data);
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
P VGShaderBase<O, P>::GetNewPassData() const
{
    return P();
}
#pragma endregion

#pragma region Pass Constant Buffer Management
template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::CreatePassCb()
{
    m_PassCb = new VGUploadBuffer<P>(m_GeneralDevice, 1, true);
}

template <typename O, typename P> requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::UpdatePassCb(void* data) const
{
    m_PassCb->CopyData(0, &data);
}
#pragma endregion
