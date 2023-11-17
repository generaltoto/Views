

#include "D3D/Geometry/VGMesh.h"
#include "D3D/Shaders/Textures/VGTexture.h"
#include "D3D/Renderers/VGMeshRenderer.h"
#include "D3D/Renderers/VGParticleRenderer.h"
#include "D3D/Base/VGHandler.h"

#include "VGShaderBase.h"

using namespace DirectX;

#pragma region SHADER BASE
VGShaderBase::VGShaderBase(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, const UINT cbvDescriptorSize, const std::wstring& filepath)
	: m_Filepath(filepath), m_GeneralDevice(device), m_CbvDescriptorSize(cbvDescriptorSize)
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

void VGShaderBase::SetInputLayout(VertexType vertexType)
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
		MessageBoxA(0, (char*)errors->GetBufferPointer(), 0, 0);
		RELPTR(errors)
	}
	ThrowIfFailed(hr)
}
#pragma endregion 

#pragma region SHADER SIMPLE
VGShaderSimple::VGShaderSimple(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath)
	: VGShaderBase(device, cbvHeap, cbvDescriptorSize, filepath)
{
}

VGShaderSimple::~VGShaderSimple()
= default;

void VGShaderSimple::Init()
{
	VGShaderBase::Init();
}

void VGShaderSimple::CreatePsoAndRootSignature(VertexType vertexType, DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat)
{
	SetInputLayout(vertexType);

	// We create our root parameters here. They are used to pass data to our shaders.
	// We have 2 root parameters here, one for our object constant buffer and one for our pass constant buffer.
	// InitAsConstantBufferView() takes the register index of the constant buffer we want to pass to our shader.
	// I.e : first parameter will be registered as b0 in our shader (b stands for buffer, because we use the InitAsConstantBufferView() method).
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(slotRootParameter), slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* serializedRootSignature = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
	ThrowIfFailed(hr)
	hr = m_GeneralDevice->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	ThrowIfFailed(hr)

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_InputLayout.data(), static_cast<UINT>(m_InputLayout.size()) };
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.VS = { reinterpret_cast<BYTE*>(m_VsByteCode->GetBufferPointer()), m_VsByteCode->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(m_PsByteCode->GetBufferPointer()), m_PsByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = rtvFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = dsvFormat;

	hr = m_GeneralDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
	ThrowIfFailed(hr)

	RELPTR(serializedRootSignature)
	RELPTR(errorBlob)
}

void VGShaderSimple::BeginDraw(ID3D12GraphicsCommandList* cmdList)
{
	// Bind the root signature to the command list
	cmdList->SetGraphicsRootSignature(m_RootSignature);

	// Bind the pass constant buffer to the pipeline at b1
	cmdList->SetGraphicsRootConstantBufferView(1, m_PassCb->GetResource()->GetGPUVirtualAddress());

	// Bind the pipeline state to the command list
	cmdList->SetPipelineState(m_PipelineState);

	// Set the primitive topology
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void VGShaderSimple::Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR)
{
	if (drawnMeshR->ObjectCbIndex >= m_ObjectCBs.size())
		AddObjectCb();

	assert(drawnMeshR->ObjectCbIndex <= m_ObjectCBs.size());

	cmdList->IASetVertexBuffers(0, 1, &drawnMeshR->Mesh->VertexBufferView());
	cmdList->IASetIndexBuffer(&drawnMeshR->Mesh->IndexBufferView());

	cmdList->SetGraphicsRootConstantBufferView(0, m_ObjectCBs[drawnMeshR->ObjectCbIndex]->GetResource()->GetGPUVirtualAddress());

	cmdList->DrawIndexedInstanced(drawnMeshR->Mesh->GetIndexCount(), 1, 0, 0, 0);
}

void VGShaderSimple::EndDraw(ID3D12GraphicsCommandList* cmdList)
{

}
#pragma endregion

#pragma region SHADER TEXTURE
VGShaderTexture::VGShaderTexture(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath)
	: VGShaderBase(device, cbvHeap, cbvDescriptorSize, filepath)
{
}

VGShaderTexture::~VGShaderTexture()
= default;

void VGShaderTexture::Init()
{
	VGShaderBase::Init();
}

void VGShaderTexture::CreatePsoAndRootSignature(VertexType vertexType, DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat)
{
	SetInputLayout(vertexType);

	// Since we need to pass a texture to our shader, we need to create a descriptor table.
	// We set its register to 0, meaning it would be accessible via the t0 register in our shader.
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// Unlike the simple shader, we have 3 root parameters here.
	// The first parameter represent our texture given to the shader as a descriptor table.
	CD3DX12_ROOT_PARAMETER slotRootParameter[3];
	slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[1].InitAsConstantBufferView(0);
	slotRootParameter[2].InitAsConstantBufferView(1);

	// Get static samplers that can be passed to the shader directly by the root signature.
	// Non static samplers must be passed to the shader via shader resources.
	auto samplers = GetStaticSamplers();

	// Create our root signature description.
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(slotRootParameter), slotRootParameter, samplers.size(), samplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* serializedRootSignature = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
	ThrowIfFailed(hr)
	hr = m_GeneralDevice->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	ThrowIfFailed(hr)

	if (errorBlob != nullptr)
	{
		// This error catching will be opening a pop up with any error inside the HLSL code.
		MessageBoxA(nullptr, static_cast<char*>(errorBlob->GetBufferPointer()), nullptr, 0);
		RELPTR(errorBlob)
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_InputLayout.data(), static_cast<UINT>(m_InputLayout.size()) };
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.VS = { reinterpret_cast<BYTE*>(m_VsByteCode->GetBufferPointer()), m_VsByteCode->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(m_PsByteCode->GetBufferPointer()), m_PsByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = rtvFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = dsvFormat;

	hr = m_GeneralDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
	ThrowIfFailed(hr)

	RELPTR(serializedRootSignature)
}

void VGShaderTexture::BeginDraw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRootSignature(m_RootSignature);

	cmdList->SetGraphicsRootConstantBufferView(2, m_PassCb->GetResource()->GetGPUVirtualAddress());

	cmdList->SetPipelineState(m_PipelineState);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void VGShaderTexture::Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR)
{
	// UINT are not supposed to be negative, it will only be negative if it is not initialized (debug only).
	// ReSharper disable once CppUnsignedZeroComparison
	assert(drawnMeshR->GetTexture(0)->GetHeapIndex() >= 0);

	// If the object has no constant buffer index, we add one.
	// This is not supposed to happen, but it is a security.
	if (drawnMeshR->ObjectCbIndex >= m_ObjectCBs.size())
		AddObjectCb();

	assert(drawnMeshR->ObjectCbIndex <= m_ObjectCBs.size());

	cmdList->IASetVertexBuffers(0, 1, &drawnMeshR->Mesh->VertexBufferView());
	cmdList->IASetIndexBuffer(&drawnMeshR->Mesh->IndexBufferView());

	ID3D12DescriptorHeap* cbvSrvHeap = nullptr;
	UINT heapSize = I(VGHandler)->GetCbvHeap(&cbvSrvHeap);
	auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	cbvHandle.Offset(drawnMeshR->GetTexture(0)->GetHeapIndex(), m_CbvDescriptorSize);

	cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);
	cmdList->SetGraphicsRootConstantBufferView(1, m_ObjectCBs[drawnMeshR->ObjectCbIndex]->GetResource()->GetGPUVirtualAddress());
	
	cmdList->DrawIndexedInstanced(drawnMeshR->Mesh->GetIndexCount(), 1, 0, 0, 0);
}

void VGShaderTexture::EndDraw(ID3D12GraphicsCommandList* cmdList)
{
}
#pragma endregion

#pragma region SHADER PARTICLE
VGShaderParticle::VGShaderParticle(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath)
	: VGShaderBase(device, cbvHeap, cbvDescriptorSize, filepath)
{
	m_ParticleInstanceDataBuffer = nullptr;
}

VGShaderParticle::~VGShaderParticle()
{
	DELPTR(m_ParticleInstanceDataBuffer);
}

void VGShaderParticle::Init()
{
	VGShaderBase::Init();

	m_ParticleInstanceDataBuffer = new VGUploadBuffer<InstanceData>(m_GeneralDevice, MAX_PARTICLE_COUNT, false);
}

void VGShaderParticle::CreatePsoAndRootSignature(VertexType vertexType, DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat)
{
	SetInputLayout(vertexType);

	CD3DX12_ROOT_PARAMETER slotRootParameter[2];
	slotRootParameter[0].InitAsShaderResourceView(0, 1);
	slotRootParameter[1].InitAsConstantBufferView(0);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(slotRootParameter), slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* serializedRootSignature = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
	ThrowIfFailed(hr)
	hr = m_GeneralDevice->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	ThrowIfFailed(hr)

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.VS = { reinterpret_cast<BYTE*>(m_VsByteCode->GetBufferPointer()), m_VsByteCode->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(m_PsByteCode->GetBufferPointer()), m_PsByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// We need to enable blending for our particles so we use alpha blending.
	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	psoDesc.BlendState = blendDesc;

	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = rtvFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = dsvFormat;

	hr = m_GeneralDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
	ThrowIfFailed(hr);

	RELPTR(serializedRootSignature);
	RELPTR(errorBlob);
}

void VGShaderParticle::BeginDraw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRootSignature(m_RootSignature);

	cmdList->SetGraphicsRootConstantBufferView(1, m_PassCb->GetResource()->GetGPUVirtualAddress());

	cmdList->SetPipelineState(m_PipelineState);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void VGShaderParticle::Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR)
{
	auto* pr = dynamic_cast<VGParticleRenderer*>(drawnMeshR);
	assert(pr);
	DrawAsParticle(cmdList, pr);
}

void VGShaderParticle::DrawAsParticle(ID3D12GraphicsCommandList* cmdList, const VGParticleRenderer* drawnMeshR)
{
	if (drawnMeshR->ObjectCbIndex >= m_ObjectCBs.size())
		AddObjectCb();

	assert(drawnMeshR->ObjectCbIndex <= m_ObjectCBs.size());

	cmdList->IASetVertexBuffers(0, 1, &drawnMeshR->Mesh->VertexBufferView());
	cmdList->IASetIndexBuffer(&drawnMeshR->Mesh->IndexBufferView());

	ID3D12DescriptorHeap* cbvSrvHeap = nullptr;
	UINT heapSize = I(VGHandler)->GetCbvHeap(&cbvSrvHeap);
	auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	cbvHandle.Offset(drawnMeshR->ObjectCbIndex, m_CbvDescriptorSize);

	cmdList->SetGraphicsRootShaderResourceView(0, m_ParticleInstanceDataBuffer->GetResource()->GetGPUVirtualAddress());

	cmdList->DrawIndexedInstanced(drawnMeshR->Mesh->GetIndexCount(), drawnMeshR->GetParticleCount(), 0, 0, 0);
}

void VGShaderParticle::EndDraw(ID3D12GraphicsCommandList* cmdList)
{

}

void VGShaderParticle::UpdateParticleInstanceDataBuffer(const int startIndex, const void* data) const
{
	m_ParticleInstanceDataBuffer->CopyData(startIndex, data);
}
#pragma endregion

#pragma region SHADER SKYBOX
VGShaderSkybox::VGShaderSkybox(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath)
	: VGShaderTexture(device, cbvHeap, cbvDescriptorSize, filepath)
{
}

VGShaderSkybox::~VGShaderSkybox()
= default;

void VGShaderSkybox::CreatePsoAndRootSignature(VertexType vertexType, DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat)
{
	SetInputLayout(vertexType);

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[3];
	slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[1].InitAsConstantBufferView(0);
	slotRootParameter[2].InitAsConstantBufferView(1);

	auto samplers = GetStaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(slotRootParameter), slotRootParameter, samplers.size(), samplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* serializedRootSignature = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
	ThrowIfFailed(hr)
	hr = m_GeneralDevice->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	ThrowIfFailed(hr)

	if (errorBlob != nullptr)
	{
		MessageBoxA(nullptr, static_cast<char*>(errorBlob->GetBufferPointer()), nullptr, 0);
		RELPTR(errorBlob)
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_InputLayout.data(), static_cast<UINT>(m_InputLayout.size()) };
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.VS = { reinterpret_cast<BYTE*>(m_VsByteCode->GetBufferPointer()), m_VsByteCode->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(m_PsByteCode->GetBufferPointer()), m_PsByteCode->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = rtvFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = dsvFormat;

	hr = m_GeneralDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
	ThrowIfFailed(hr)

	RELPTR(serializedRootSignature)
}
#pragma endregion

#pragma region SHADER TEXTURE OFFSET
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

#pragma endregion