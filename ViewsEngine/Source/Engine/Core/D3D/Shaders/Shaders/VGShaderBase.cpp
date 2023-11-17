#include "VGShaderBase.h"

using namespace DirectX;

VGShaderBase::VGShaderBase(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, const UINT cbvDescriptorSize, std::wstring filepath)
	: m_Filepath(std::move(filepath)), m_GeneralDevice(device), m_CbvDescriptorSize(cbvDescriptorSize)
{
	m_PassCb = nullptr;

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

	for (auto& cb : m_ObjectCBs)
		DELPTR(cb)

	m_ObjectCBs.clear();
	DELPTR(m_PassCb)
}

void VGShaderBase::Init()
{
	CreatePassCb();

	CompileShader(nullptr, "vs_main", "vs_5_1", &m_VsByteCode);
	CompileShader(nullptr, "ps_main", "ps_5_1", &m_PsByteCode);
}

void VGShaderBase::SetInputLayout()
{
	// Input Layout represents the data we are sending to the shader via the VS_INPUT struct.
	m_InputLayout.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	m_InputLayout.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	m_InputLayout.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	m_InputLayout.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	m_InputLayout.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> VGShaderBase::GetStaticSamplers()
{
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW


	return { pointWrap, linearWrap };
}

void VGShaderBase::UnBind(const UINT index)
{
	DELPTR(m_ObjectCBs[index])
	m_FreeIndices.push(index);
}

VGShaderBase* VGShaderBase::Bind()
{
	AddObjectCb();
	return this;
}

void VGShaderBase::AddObjectCb() 
{ 
	auto ub = new VGUploadBuffer<ObjConstants>(m_GeneralDevice, 1, true);
	m_ObjectCBs.push_back(ub);
	NULLPTR(ub)
}

void VGShaderBase::UpdateObjectCb(DirectX::XMFLOAT4X4* itemWorldMatrix, UINT cbIndex)
{
	if (cbIndex >= m_ObjectCBs.size())
		AddObjectCb();

	ObjConstants objConstants;
	objConstants.World = *itemWorldMatrix;
	m_ObjectCBs[cbIndex]->CopyData(0, &objConstants);
}

void VGShaderBase::CreatePassCb() { m_PassCb = new VGUploadBuffer<PassConstants>(m_GeneralDevice, 1, true); }

void VGShaderBase::UpdatePassCb(const float dt, const float totalTime) const
{
	const XMMATRIX camView = CameraManager::GetMainCamera()->GetView();
	const XMMATRIX camOrthoProj = CameraManager::GetMainCamera()->GetOrthoProj();
	const XMMATRIX camViewProj = CameraManager::GetMainCamera()->GetViewProj();

	PassConstants mainPassCB;

	XMStoreFloat4x4(&mainPassCB.View, XMMatrixTranspose(camView));
	XMStoreFloat4x4(&mainPassCB.OrthoProj, XMMatrixTranspose(camOrthoProj));
	XMStoreFloat4x4(&mainPassCB.ViewProj, XMMatrixTranspose(camViewProj));

	mainPassCB.LightColor = XMFLOAT4(0.9f, 0.7f, 0.7f, 1.0f);
	mainPassCB.LightDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	mainPassCB.EyePosW = CameraManager::GetMainCamera()->transform->m_pParent->GetPosition();
	mainPassCB.TotalTime = totalTime;
	mainPassCB.DeltaTime = dt;

	m_PassCb->CopyData(0, &mainPassCB);
}

void VGShaderBase::CompileShader(const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, ID3DBlob** uploader) const
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
