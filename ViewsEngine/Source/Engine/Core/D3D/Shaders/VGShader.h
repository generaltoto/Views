#pragma once

enum class CBufferType : UINT
{
    OBJECT,
    PASS,
    MATERIAL
};

struct CBufferProperty
{
    void* Data;
    UINT Size;
};

// This class servers as a base for all shaders. It it used for creating shader list without the VGShader template.
class VGShaderBase
{
public:
    VGShaderBase(ID3D12Device* device, const UINT cbvDescriptorSize, std::wstring filepath);
    virtual ~VGShaderBase();

    virtual void Init();
    virtual void UnBind(const UINT index) = 0;
    virtual VGShaderBase* Bind() = 0;

    virtual void CreatePsoAndRootSignature(DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) = 0;

    virtual UINT GetCreatedIndex(CBufferType bType) = 0;
    virtual UINT GetLastIndex(CBufferType bType) = 0;

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

    void SetProperty(CBufferType bType, const std::string& name, void* data);
    std::unordered_map<CBufferType, std::unordered_map<std::string, CBufferProperty>> m_CBuffers;

protected:
    void SetInputLayout();

    static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> GetStaticSamplers();

    void CompileShader(const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, ID3DBlob** uploader) const;
};
