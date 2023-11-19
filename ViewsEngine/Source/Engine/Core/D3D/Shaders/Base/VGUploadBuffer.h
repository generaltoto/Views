#pragma once

/*
This class serves as a wrapper for the ID3D12Resource that is used to upload data to the GPU.
*/
template<typename T>
class VGUploadBuffer
{
public:
	VGUploadBuffer(ID3D12Device* device, UINT elementCount, BOOL isConstantBuffer);
	~VGUploadBuffer();

	VGUploadBuffer(const VGUploadBuffer& rhs) = delete;
	VGUploadBuffer& operator=(const VGUploadBuffer& rhs) = delete;

	[[nodiscard]] UINT GetElementByteSize() const { return m_ElementByteSize; }
	[[nodiscard]] ID3D12Resource* GetResource() const { return m_UploadBuffer; }
	[[nodiscard]] BYTE* GetMappedData() const { return m_mappedData; }

	void CopyData(int elementIndex, const void* data) const;

private:
	ID3D12Resource* m_UploadBuffer;
	UINT m_ElementByteSize;
	BYTE* m_mappedData;
	BOOL m_isConstantBuffer;
};

template<typename T>
VGUploadBuffer<T>::VGUploadBuffer(ID3D12Device* device, UINT elementCount, BOOL isConstantBuffer) :
	m_ElementByteSize(0), m_mappedData(nullptr), m_isConstantBuffer(isConstantBuffer)
{
	m_ElementByteSize = sizeof(T);

	if (isConstantBuffer)
		m_ElementByteSize = (sizeof(T) + 255) & ~255;

	const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_ElementByteSize * elementCount);
	HRESULT hr =
		device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_UploadBuffer));
	ThrowIfFailed(hr)

	hr = m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData));
	ThrowIfFailed(hr)
}


template<typename T>
VGUploadBuffer<T>::~VGUploadBuffer()
{
	if (m_UploadBuffer != nullptr)
		m_UploadBuffer->Unmap(0, nullptr);

	m_mappedData = nullptr;
}

template<typename T>
void VGUploadBuffer<T>::CopyData(int elementIndex, const void* data) const
{
	memcpy(&m_mappedData[elementIndex * m_ElementByteSize], data, sizeof(T));
}
