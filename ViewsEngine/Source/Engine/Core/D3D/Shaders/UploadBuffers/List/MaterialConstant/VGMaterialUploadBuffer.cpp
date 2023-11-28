#include "VGMaterialUploadBuffer.h"

#include "D3D/Base/VGHandler.h"

VGMaterialUploadBuffer::VGMaterialUploadBuffer() :
    m_ElementByteSize(0), m_IsConstantBuffer(true)
{
    CreateResourceAndGetData(0);
}

VGMaterialUploadBuffer::~VGMaterialUploadBuffer()
{
    FreeData();
}

void VGMaterialUploadBuffer::FreeData()
{
    if (m_UploadBuffer != nullptr)
        m_UploadBuffer->Unmap(0, nullptr);

    m_MappedData = nullptr;
}

void VGMaterialUploadBuffer::CreateResourceAndGetData(const UINT size)
{
    FreeData();
	
    ID3D12Device* device = I(VGHandler)->GetDevice();
    m_MappedData = nullptr;
    m_ElementByteSize = m_IsConstantBuffer ? (size + 255) & ~255 : size;

    const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
    const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_ElementByteSize);
    HRESULT hr =
        device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_UploadBuffer));
    ThrowIfFailed(hr)

    hr = m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData));
    ThrowIfFailed(hr)
}

void VGMaterialUploadBuffer::CopyData(const UINT elementIndex, const void* data, const size_t size) const
{
    memcpy(&m_MappedData[elementIndex * m_ElementByteSize], data, size);  // NOLINT(bugprone-implicit-widening-of-multiplication-result)lll
}

CBProperty& VGMaterialUploadBuffer::FindProperty(const std::string& name)
{
    const auto it = m_Properties.find(name);
    if (it == m_Properties.end())
        throw std::exception("Property not found");

    return it->second;
}

void VGMaterialUploadBuffer::SetProperty(const std::string& name, const void* data)
{
    // Get the property we want to update
    const auto& [ByteIndex, ByteSize] = FindProperty(name);

    // Make sure the data we want to update is the same size as the property
    assert(ByteSize == sizeof(data));

    // Copy the data to the mapped data
    memcpy(&m_MappedData[ByteIndex], data, ByteSize);
}

void VGMaterialUploadBuffer::AddProperty(const std::string& name, const UINT byteSize)
{
    // Make sure the property doesn't already exist
    assert(m_Properties.contains(name) == false);

    // Make sure the property is not bigger than a float4x4 and is a multiple of 4
    assert(byteSize <= 64 && byteSize % 4 == 0);

    // Add the property to the map
    m_Properties[name] = {m_ElementByteSize, byteSize};

    // Add the property to the upload buffer
    CopyData(0, &byteSize, sizeof(byteSize));
    
    // Update the upload buffer size
    m_ElementByteSize += byteSize;
}
