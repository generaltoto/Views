#include "VGShaderTexture.h"

#include "D3D/Geometry/VGMesh.h"
#include "D3D/Base/VGHandler.h"

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

void VGShaderTexture::CreatePsoAndRootSignature(DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat)
{
	SetInputLayout();

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
	if (drawnMeshR->m_ObjectCbIndex >= m_ObjectCBs.size())
		AddObjectCb();

	assert(drawnMeshR->m_ObjectCbIndex <= m_ObjectCBs.size());

	cmdList->IASetVertexBuffers(0, 1, &drawnMeshR->m_Mesh->VertexBufferView());
	cmdList->IASetIndexBuffer(&drawnMeshR->m_Mesh->IndexBufferView());

	ID3D12DescriptorHeap* cbvSrvHeap = nullptr;
	UINT heapSize = I(VGHandler)->GetCbvHeap(&cbvSrvHeap);
	auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	cbvHandle.Offset(drawnMeshR->GetTexture(0)->GetHeapIndex(), m_CbvDescriptorSize);

	cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);
	cmdList->SetGraphicsRootConstantBufferView(1, m_ObjectCBs[drawnMeshR->m_ObjectCbIndex]->GetResource()->GetGPUVirtualAddress());
	
	cmdList->DrawIndexedInstanced(drawnMeshR->m_Mesh->GetIndexCount(), 1, 0, 0, 0);
}

void VGShaderTexture::EndDraw(ID3D12GraphicsCommandList* cmdList)
{
}
