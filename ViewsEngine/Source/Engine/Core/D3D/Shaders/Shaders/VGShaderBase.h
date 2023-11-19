#pragma once
#include <stack>

#include "D3D/Base/VGUtils.h"
#include "D3D/Shaders/Base/VGUploadBuffer.h"

class VGIRenderer;

struct ObjConstants
{
    DirectX::XMFLOAT4X4 World;
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

template <typename O = ObjConstants, typename P = PassConstants>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
class VGShaderBase
{
public:
    VGShaderBase(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, const UINT cbvDescriptorSize, std::wstring filepath);
    virtual ~VGShaderBase();

public:
    virtual void Init();

    virtual void CreatePsoAndRootSignature(DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) = 0;

    virtual void BeginDraw(ID3D12GraphicsCommandList* cmdList) = 0;
    virtual void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) = 0;
    virtual void EndDraw(ID3D12GraphicsCommandList* cmdList) = 0;

    virtual UINT GetCreatedIndex() { return static_cast<UINT>(m_ObjectCBs.size()) - 1; }
    virtual UINT GetLastIndex() { return static_cast<UINT>(m_ObjectCBs.size()); }

    void UnBind(const UINT index);
    VGShaderBase* Bind();

    virtual void AddObjectCb();
    virtual void UpdateObjectCb(O& objData, UINT cbIndex);
    O GetNewObjectData() const { return O(); }

    void CreatePassCb();
    void UpdatePassCb(P& passData) const;
    P GetNewPassData() const { return P(); }

    void CompileShader(const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, ID3DBlob** uploader) const;
protected:
    std::wstring m_Filepath;

    std::vector<VGUploadBuffer<O>*> m_ObjectCBs;
    std::stack<UINT> m_FreeIndices;
    VGUploadBuffer<P>* m_PassCb;

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
};

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
VGShaderBase<O, P>::VGShaderBase(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, const UINT cbvDescriptorSize, std::wstring filepath)
        : m_Filepath(std::move(filepath)), m_GeneralDevice(device), m_CbvDescriptorSize(cbvDescriptorSize)
{
    m_PassCb = nullptr;

    m_RootSignature = nullptr;
    m_PipelineState = nullptr;

    m_VsByteCode = nullptr;
    m_PsByteCode = nullptr;
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
VGShaderBase<O, P>::~VGShaderBase()
{
    NULLPTR(m_GeneralDevice)

    RELPTR(m_PipelineState)
    RELPTR(m_RootSignature)

    RELPTR(m_VsByteCode)
    RELPTR(m_PsByteCode)

    for (auto& cb : m_ObjectCBs)
        DELPTR(cb)

    m_ObjectCBs.clear();
    DELPTR(m_PassCb)
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O,P>::Init()
{
    CreatePassCb();

    CompileShader(nullptr, "vs_main", "vs_5_1", &m_VsByteCode);
    CompileShader(nullptr, "ps_main", "ps_5_1", &m_PsByteCode);
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::UnBind(const UINT index)
{
    DELPTR(m_ObjectCBs[index])
    m_FreeIndices.push(index);
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
VGShaderBase<O, P>* VGShaderBase<O, P>::Bind()
{
    AddObjectCb();
    return this;
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::AddObjectCb() 
{ 
    auto ub = new VGUploadBuffer<O>(m_GeneralDevice, 1, true);
    m_ObjectCBs.push_back(ub);
    NULLPTR(ub)
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::UpdateObjectCb(O& objData, UINT cbIndex)
{
    if (cbIndex >= m_ObjectCBs.size())
        AddObjectCb();

    m_ObjectCBs[cbIndex]->CopyData(0, &objData);
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::CreatePassCb()
{
    m_PassCb = new VGUploadBuffer<P>(m_GeneralDevice, 1, true);
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::UpdatePassCb(P& passData) const
{
    m_PassCb->CopyData(0, &passData);
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::CompileShader(const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, ID3DBlob** uploader) const
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	
	ID3DBlob* errors = nullptr;

	const HRESULT hr =
		D3DCompileFromFile(m_Filepath.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(), compileFlags, 0, uploader, &errors);
	ThrowIfFailed(hr)
	
	if (errors != nullptr)
	{
		MessageBoxA(nullptr, static_cast<char*>(errors->GetBufferPointer()), nullptr, 0);
		RELPTR(errors)
	}
	ThrowIfFailed(hr)
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
void VGShaderBase<O, P>::SetInputLayout()
{
    // Input Layout represents the data we are sending to the shader via the VS_INPUT struct.
    m_InputLayout.push_back({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    m_InputLayout.push_back({"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    m_InputLayout.push_back({"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    m_InputLayout.push_back({"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    m_InputLayout.push_back({"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
}

template <typename O, typename P>
requires std::is_base_of_v<ObjConstants, O> && std::is_base_of_v<PassConstants, P>
std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> VGShaderBase<O, P>::GetStaticSamplers()
{
    const CD3DX12_STATIC_SAMPLER_DESC pointWrap(0, // shaderRegister
                                                D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
                                                D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
                                                D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
                                                D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearWrap(1, // shaderRegister
                                                 D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
                                                 D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
                                                 D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
                                                 D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW


    return {pointWrap, linearWrap};
}

