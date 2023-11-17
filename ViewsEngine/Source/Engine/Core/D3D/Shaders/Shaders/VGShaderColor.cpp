#include "VGShaderColor.h"

#include "D3D/Geometry/VGMesh.h"

#pragma region SHADER SIMPLE
VGShaderColor::VGShaderColor(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath)
	: VGShaderBase(device, cbvHeap, cbvDescriptorSize, filepath)
{
}

VGShaderColor::~VGShaderColor()
= default;

void VGShaderColor::Init()
{
	VGShaderBase::Init();
}

void VGShaderColor::CreatePsoAndRootSignature(DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat)
{
	SetInputLayout();

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

void VGShaderColor::BeginDraw(ID3D12GraphicsCommandList* cmdList)
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

void VGShaderColor::Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR)
{
	if (drawnMeshR->ObjectCbIndex >= m_ObjectCBs.size())
		AddObjectCb();

	assert(drawnMeshR->ObjectCbIndex <= m_ObjectCBs.size());

	cmdList->IASetVertexBuffers(0, 1, &drawnMeshR->Mesh->VertexBufferView());
	cmdList->IASetIndexBuffer(&drawnMeshR->Mesh->IndexBufferView());

	cmdList->SetGraphicsRootConstantBufferView(0, m_ObjectCBs[drawnMeshR->ObjectCbIndex]->GetResource()->GetGPUVirtualAddress());

	cmdList->DrawIndexedInstanced(drawnMeshR->Mesh->GetIndexCount(), 1, 0, 0, 0);
}

void VGShaderColor::EndDraw(ID3D12GraphicsCommandList* cmdList)
{

}
#pragma endregion
