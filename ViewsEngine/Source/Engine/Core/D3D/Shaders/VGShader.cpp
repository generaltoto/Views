#include "VGShader.h"

VGShaderBase::VGShaderBase(ID3D12Device* device, const UINT cbvDescriptorSize, std::wstring filepath)
    : m_Filepath(std::move(filepath)), m_GeneralDevice(device), m_CbvDescriptorSize(cbvDescriptorSize)
{
    m_RootSignature = nullptr;
    m_PipelineState = nullptr;

    m_VsByteCode = nullptr;
    m_PsByteCode = nullptr;
}

VGShaderBase::~VGShaderBase()
{
    NULLPTR(m_GeneralDevice)

    RELPTR(m_PipelineState)
    RELPTR(m_RootSignature)

    RELPTR(m_VsByteCode)
    RELPTR(m_PsByteCode)
}

void VGShaderBase::Init()
{
    CompileShader(nullptr, "vs_main", "vs_5_1", &m_VsByteCode);
    CompileShader(nullptr, "ps_main", "ps_5_1", &m_PsByteCode);
}

void VGShaderBase::SetProperty(CBufferType bType, const std::string& name, void* data)
{
}

void VGShaderBase::SetInputLayout()
{
    // Input Layout represents the data we are sending to the shader via the VS_INPUT struct.
    m_InputLayout.push_back({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    m_InputLayout.push_back({"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    m_InputLayout.push_back({"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    m_InputLayout.push_back({"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
    m_InputLayout.push_back({"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> VGShaderBase::GetStaticSamplers()
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

void VGShaderBase::CompileShader(const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, ID3DBlob** uploader) const
{
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* errors = nullptr;

    const HRESULT hr = D3DCompileFromFile(m_Filepath.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(),
                                          compileFlags, 0, uploader, &errors);
    ThrowIfFailed(hr)

    if (errors != nullptr)
    {
        MessageBoxA(nullptr, static_cast<char*>(errors->GetBufferPointer()), nullptr, 0);
        RELPTR(errors)
    }
    ThrowIfFailed(hr)
}

