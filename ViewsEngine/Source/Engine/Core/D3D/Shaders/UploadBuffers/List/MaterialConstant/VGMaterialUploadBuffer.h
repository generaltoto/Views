#pragma once

struct CBProperty
{
	UINT ByteIndex = 0;
	UINT ByteSize  = 0;
};

enum SupportedTypeSize
{
	F4_X_4	= 64,
	F4		= 16,
	F3		= 12,
	F2		= 8,
	F1		= 4
};

class VGMaterialUploadBuffer
{
public:
	VGMaterialUploadBuffer();
	~VGMaterialUploadBuffer();

	VGMaterialUploadBuffer(const VGMaterialUploadBuffer& rhs) = delete;
	VGMaterialUploadBuffer& operator=(const VGMaterialUploadBuffer& rhs) = delete;

	[[nodiscard]] UINT GetElementByteSize() const { return m_ElementByteSize; }
	[[nodiscard]] ID3D12Resource* GetResource() const { return m_UploadBuffer; }
	[[nodiscard]] BYTE* GetMappedData() const { return m_MappedData; }

	void FreeData();
	void CreateResourceAndGetData(UINT size);
	void CopyData(UINT elementIndex, const void* data, size_t size) const;

	CBProperty& FindProperty(const std::string& name);
    
	void SetProperty(const std::string& name, const void* data);
	void AddProperty(const std::string& name, UINT byteSize);

private:
	ID3D12Resource* m_UploadBuffer;
	UINT m_ElementByteSize;
	BYTE* m_MappedData;
	BOOL m_IsConstantBuffer;

	std::unordered_map<std::string, CBProperty> m_Properties;
};
