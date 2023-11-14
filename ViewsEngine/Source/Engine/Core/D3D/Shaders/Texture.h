#pragma once 

#include "Engine/IResourceObject.h"

struct ID3D12Resource;

class Texture : public IResourceObject
{
public:
	explicit Texture(const std::string& name);
	~Texture() override;
	std::string Name;
	ID3D12Resource* Resource = nullptr;
	ID3D12Resource* UploadHeap = nullptr;

	UINT HeapIndex;

	void Load(const std::string& filepath) override;

protected:
	int m_textType;

private:
	void LoadTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
};