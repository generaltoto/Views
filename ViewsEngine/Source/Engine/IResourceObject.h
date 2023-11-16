#pragma once

struct ID3D12Resource;

class IResourceObject
{
public:
	explicit IResourceObject(std::string name);
	virtual ~IResourceObject();

	virtual void Load(const std::string& filepath) = 0;

	[[nodiscard]] std::string GetName() const { return m_Name; }
	[[nodiscard]] ID3D12Resource* GetResource() const { return m_Resource; }
	[[nodiscard]] UINT GetHeapIndex() const { return m_HeapIndex; }
	[[nodiscard]] int GetResType() const { return m_ResType; }

protected:
	std::string m_Filepath;
	std::string m_Name;

	ID3D12Resource* m_Resource;
	ID3D12Resource* m_UploadHeap;

	UINT m_HeapIndex;
	int m_ResType;
};