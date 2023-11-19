#include "VGShaderParticle.h"

#include "D3D/Geometry/VGMesh.h"
#include "D3D/Base/VGHandler.h"

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

	m_ParticleInstanceDataBuffer = new VGUploadBuffer<ParticleObjConstants>(m_GeneralDevice, MAX_PARTICLE_COUNT, false);
}

void VGShaderParticle::CreatePsoAndRootSignature(DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat)
{
	SetInputLayout();

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