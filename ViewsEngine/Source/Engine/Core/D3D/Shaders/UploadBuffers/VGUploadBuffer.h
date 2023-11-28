#pragma once
#include "D3D/Base/VGHandler.h"

/*
This class serves as a wrapper for the ID3D12Resource that is used to upload data to the GPU.
*/
template<typename T>
class VGUploadBuffer
{
public:
	VGUploadBuffer(UINT elementCount, BOOL isConstantBuffer);
	virtual ~VGUploadBuffer();

	VGUploadBuffer(const VGUploadBuffer& rhs) = delete;
	VGUploadBuffer& operator=(const VGUploadBuffer& rhs) = delete;

	[[nodiscard]] UINT GetElementByteSize() const { return m_ElementByteSize; }
	[[nodiscard]] ID3D12Resource* GetResource() const { return m_UploadBuffer; }
	[[nodiscard]] BYTE* GetMappedData() const { return m_MappedData; }

	void FreeData();
	void CreateResourceAndGetData(UINT size, UINT elementCount);
	void CopyData(int elementIndex, const void* data) const;

protected:
	ID3D12Resource* m_UploadBuffer;
	UINT m_ElementByteSize;
	BYTE* m_MappedData;
	BOOL m_IsConstantBuffer;
};

template<typename T>
VGUploadBuffer<T>::VGUploadBuffer(const UINT elementCount, const BOOL isConstantBuffer) :
	m_ElementByteSize(0), m_IsConstantBuffer(isConstantBuffer)
{
	CreateResourceAndGetData(sizeof(T), elementCount);
}


template<typename T>
VGUploadBuffer<T>::~VGUploadBuffer()
{
	if (m_UploadBuffer != nullptr)
		m_UploadBuffer->Unmap(0, nullptr);

	m_MappedData = nullptr;
}

template <typename T>
void VGUploadBuffer<T>::FreeData()
{
	if (m_UploadBuffer != nullptr)
		m_UploadBuffer->Unmap(0, nullptr);

	m_MappedData = nullptr;
}

template <typename T>
void VGUploadBuffer<T>::CreateResourceAndGetData(const UINT size, const UINT elementCount)
{
	FreeData();
	
	ID3D12Device* device = I(VGHandler)->GetDevice();
	m_MappedData = nullptr;
	m_ElementByteSize = m_IsConstantBuffer ? (size + 255) & ~255 : size;

	const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_ElementByteSize * elementCount);
	HRESULT hr =
		device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_UploadBuffer));
	ThrowIfFailed(hr)

	hr = m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData));
	ThrowIfFailed(hr)
}

template<typename T>
void VGUploadBuffer<T>::CopyData(const int elementIndex, const void* data) const
{
	memcpy(&m_MappedData[elementIndex * m_ElementByteSize], data, sizeof(T));
}
