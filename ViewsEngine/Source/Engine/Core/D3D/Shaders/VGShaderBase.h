#pragma once
#include <stack>

#include "VGUploadBuffer.h"

class VGTexture;
class VGParticleRenderer;
class VGMeshRenderer;
struct InstanceData;

enum VertexType { VERTEX };

class VGShaderBase
{
public:
	VGShaderBase(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, const std::wstring& filepath);
	virtual ~VGShaderBase();

protected:
	struct ObjConstants
	{
		DirectX::XMFLOAT4X4 World;
	};

	struct PassConstants
	{
		DirectX::XMFLOAT4X4 View = Identity4X4();
		DirectX::XMFLOAT4X4 OrthoProj = Identity4X4();
		DirectX::XMFLOAT4X4 ViewProj = Identity4X4();

		DirectX::XMFLOAT4 LightColor = { 1.0f, 0.0f, 1.0f, 1.0f };

		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float TotalTime = 0.0f;

		DirectX::XMFLOAT3 LightDirection = { -1.0f, -1.0f, 0.0f };
		float DeltaTime = 0.0f;
	};

	struct MaterialConstants
	{
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = .25f;

		// Used in texture mapping.
		DirectX::XMFLOAT4X4 MatTransform = Identity4X4();
	};

protected:
	std::wstring m_Filepath;

	/* Upload buffers are used to give the GPU information at runtime with the CPU.
	Those buffers uses the GPU Upload Heap that allows the CPU to upload data to the GPU at runtime.

	m_objectCBs stores every object constant buffer. Each of them is associated to a unique MeshRenderer
	Constant buffers are bound to the shader using the InitAsConstantBuffer() method in rootParameters declaration
	To give them to the shader during draw, use the SetGraphicsRootConstantBufferView() method */
	std::vector<VGUploadBuffer<ObjConstants>*> m_ObjectCBs;
	std::stack<UINT> m_FreeIndices;
	/* m_passCB stores every information the shader might need about our game
	The main pass constant buffer is associated to the b1 cbuffer in the shader (only true in our project) */
	VGUploadBuffer<PassConstants>* m_PassCb;
	/* This struct helps the GPU identifying how our Vertex class is composed
	This information will be used by the shader as the VertexIn struct*/
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	/* D3D12RootSignature : Defines where the resources bound to the rendering pipeline can be found by the shader
	We use a root signature to define the resources that are going to be used by the shaders
	Therefore, the root signature will be created with an array of RootParameter that express where the expected resource by the shader is located */
	ID3D12RootSignature* m_RootSignature;

	/* Represents the compiled shader code for both vs and ps functions (HLSL) */
	ID3D10Blob* m_VsByteCode;
	ID3D10Blob* m_PsByteCode;

	/* D3D12 PipelineStateObject : (PSO : Pipeline State Object) Represents the state of the pipeline
	We use a PSO to define the state of the pipeline. This includes the shaders, the input layout, the render targets, the depth stencil buffer, etc...
	For each shader, we need to create another PSO, this sytem will be implemented later on */
	ID3D12PipelineState* m_PipelineState;

	ID3D12Device* m_GeneralDevice;
	UINT m_CbvDescriptorSize;

public:
	virtual void Init() = 0;
	virtual void CreatePsoAndRootSignature(VertexType vertexType, DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) = 0;

	virtual void BeginDraw(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) = 0;
	virtual void EndDraw(ID3D12GraphicsCommandList* cmdList) = 0;

	virtual UINT GetCreatedIndex() { return static_cast<UINT>(m_ObjectCBs.size()) - 1; }
	virtual UINT GetLastIndex() { return static_cast<UINT>(m_ObjectCBs.size()); }

	void UnBind(UINT index);
	VGShaderBase* Bind();

	virtual void AddObjectCb();
	virtual void UpdateObjectCb(DirectX::XMFLOAT4X4* itemWorldMatrix, UINT cbIndex);

	void CreatePassCb();
	void UpdatePassCb(const float dt, const float totalTime) const;

	void CompileShader(const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target, ID3DBlob** uploader) const;

protected:
	void SetInputLayout(VertexType vertexType);

	/* Samplers defines how we want to parse our texture in our shader.
	Creating multiple Samplers allows us to have access to different parse mode in our shader later on. This is useful on big engines.
	Note that will certainly do not need that much samplers, but it was juste to learn how to use them. */
	static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 2> GetStaticSamplers();
};

class VGShaderSimple final : public VGShaderBase
{
public:
	VGShaderSimple(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath);
	~VGShaderSimple() override;

	void Init() override;
	void CreatePsoAndRootSignature(VertexType vertexType, DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) override;

	void BeginDraw(ID3D12GraphicsCommandList* cmdList) override;
	void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) override;
	void EndDraw(ID3D12GraphicsCommandList* cmdList) override;
};

class VGShaderTexture : public VGShaderBase
{
public:
	VGShaderTexture(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath);
	~VGShaderTexture() override;

	void Init() override;
	void CreatePsoAndRootSignature(VertexType vertexType, DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) override;

	void BeginDraw(ID3D12GraphicsCommandList* cmdList) override;
	void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) override;
	void EndDraw(ID3D12GraphicsCommandList* cmdList) override;
};

class VGShaderParticle final : public VGShaderBase
{
public:
	VGShaderParticle(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath);
	~VGShaderParticle() override;

	void Init() override;
	void CreatePsoAndRootSignature(VertexType vertexType, DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) override;

	void BeginDraw(ID3D12GraphicsCommandList* cmdList) override;
	void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) override;
	void EndDraw(ID3D12GraphicsCommandList* cmdList) override;

	void UpdateParticleInstanceDataBuffer(int startIndex, const void* data) const;
private:
	void DrawAsParticle(ID3D12GraphicsCommandList* cmdList, const VGParticleRenderer* drawnMeshR);

	/*
	To render particles with a minimum amount of draw calls, we need to use instancing.
	Instancing allows us to draw multiple instances of the same mesh with only one draw call.
	
	To do so, we need to create a buffer that will store the data of each instance.
	We can then give the buffer to the shader that will catch it in a StructuredBuffer. We also pass the instanceID to the shader via the vs_main method.
	See Texture_UI.hlsl and chapter 16.1 for more information.
	*/
	VGUploadBuffer<InstanceData>* m_ParticleInstanceDataBuffer;
};

class VGShaderSkybox final : public VGShaderTexture
{
public:
	VGShaderSkybox(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath);
	~VGShaderSkybox() override;

	void CreatePsoAndRootSignature(VertexType vertexType, DXGI_FORMAT& rtvFormat, DXGI_FORMAT& dsvFormat) override;
};

class VGShaderTextureUI final : public VGShaderTexture
{
public:
	struct OffSetConstants
	{
		DirectX::XMFLOAT4X4 World = Identity4X4();
		float UVOffsetY = 0.0f;
	};

public:
	VGShaderTextureUI(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvDescriptorSize, std::wstring& filepath);
	~VGShaderTextureUI() override;

	UINT GetCreatedIndex() override { return static_cast<UINT>(m_OffSetCb.size()) - 1; }

	void Draw(ID3D12GraphicsCommandList* cmdList, VGIRenderer* drawnMeshR) override;
	void UpdateAsOffset(const DirectX::XMFLOAT4X4* itemWorldMatrix, UINT cbIndex, float offSetY);

protected:
	void AddObjectCb() override;

	/* We created a new object constant buffer to store the offset value of the UI element. */
	std::vector<VGUploadBuffer<OffSetConstants>*> m_OffSetCb;
};