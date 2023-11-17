#include "Engine/IResourceObject.h"

#include "VGTexture.h"

#include "D3D/Base/VGHandler.h"

#include "D3D/External/DDSTextureLoader.h"
#include <wrl/client.h>

using namespace Microsoft::WRL;
using namespace DirectX;

VGTexture::VGTexture(const std::string& filename) : IResourceObject(filename)
{
	m_ResType = D3D12_SRV_DIMENSION_TEXTURE2D;
}

VGTexture::~VGTexture() = default;

void VGTexture::Load(const std::string& filepath)
{
	m_Filepath = filepath;

	LoadTexture();
}

void VGTexture::LoadTexture()
{
	const auto app = I(VGHandler);
	ID3D12Device* device = app->GetDevice();
	ID3D12GraphicsCommandList* cmdList = app->GetCommandList();

	app->BeginList();
	
	// This is a bit ugly, but CreateDDSTextureFromFile12 needs ComPtr, which we do not use in the rest of the project.
	ComPtr<ID3D12Resource> textureUploadHeap = m_UploadHeap;
	ComPtr<ID3D12Resource> textureResource = m_Resource;

	// I hate this. But idk how to convert from std::string to const wchar_t*. Please help
	const auto wstr = std::wstring(m_Filepath.begin(), m_Filepath.end());
	const HRESULT hr = CreateDDSTextureFromFile12(device, cmdList, wstr.c_str(), textureResource, textureUploadHeap);
	ThrowIfFailed(hr)

	m_Resource = textureResource.Detach();
	m_UploadHeap = textureUploadHeap.Detach();

	m_HeapIndex = Resource::AddToResourceHeap(this, m_ResType);

	app->EndList();
}
