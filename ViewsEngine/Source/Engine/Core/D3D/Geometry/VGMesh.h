#pragma once 

struct VGVertex
{
	VGVertex();
	VGVertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT4 c, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT3& t, const DirectX::XMFLOAT2& uv);
	VGVertex(
		float px, float py, float pz,
		float cr, float cg, float cb, float ca,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v
	);
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 TangentU;
	DirectX::XMFLOAT2 TexC;
};

/*
This class stores the vertex and index buffers for a geometric mesh.
It will be used as a buffer to prevent the need to create a new vertex and index for each object.
*/
class VGMesh
{
public:
	VGMesh();
	~VGMesh();

	std::string Name;

public:
	[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
	[[nodiscard]] D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

	void DisposeUploader();

	void Create(const void* vData, UINT vStride, UINT vSize, const void* iData, UINT iStride, UINT iSize);

	[[nodiscard]] UINT GetIndexCount() const { return m_IndexCount; }

private:
	ID3D12Resource* m_pVertexBufferGpu;
	ID3D12Resource* m_pIndexBufferGpu;

	ID3D12Resource* m_pVertexBufferUploader;
	ID3D12Resource* m_pIndexBufferUploader;

	UINT m_VertexByteStride;
	UINT m_VertexBufferByteSize;
	DXGI_FORMAT m_IndexFormat;
	UINT m_IndexBufferByteSize;

	UINT m_IndexCount = 0;
	UINT m_StartIndexLocation = 0;
	INT m_BaseVertexLocation = 0;

private:
	ID3D12Resource* CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize, ID3D12Resource*& uploadBuffer);

};

