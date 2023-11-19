#include "D3D/Shaders/Shaders/VGShaderBase.h"
#include "D3D/Shaders/Shaders/VGShaderColor.h"
#include "D3D/Shaders/Shaders/VGShaderTexture.h"
#include "D3D/Shaders/Shaders/VGShaderTextureUI.h"
#include "D3D/Shaders/Shaders/VGShaderParticle.h"
#include "D3D/Shaders/Shaders/VGShaderSkybox.h"

#include "D3D/Shaders/Base/VGMaterial.h"
#include "IResourceObject.h"

#include "Resource.h"

#include <ranges>

#include "D3D/Base/VGHandler.h"

std::unordered_map<MaterialType, VGShaderBase*> Resource::m_Shaders;
std::unordered_map<MaterialType, VGMaterial*> Resource::m_Materials;
std::unordered_map<std::string, IResourceObject*> Resource::m_Resources;

int Resource::m_TexIndex = 0;

Resource::~Resource()
= default;

VGMaterial* Resource::LoadMaterial(const MaterialType matType)
{
	return m_Materials[matType];
}

void Resource::CreateResources(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, const UINT cbvSrvDescriptorSize)
{
	CreateShaders(device, cbvHeap, cbvSrvDescriptorSize);
	CreateMaterials();
}

void Resource::ReleaseResources()
{
	for (auto& mat : m_Materials | std::views::values)
	{
		DELPTR(mat)
	}

	for (auto& shader : m_Shaders | std::views::values)
	{
		DELPTR(shader)
	}

	for (auto& res : m_Resources | std::views::values)
	{
		DELPTR(res)
	}
	
	m_Shaders.clear();
	m_Materials.clear();
	m_Resources.clear();
}

VGShaderBase* Resource::FindShader(const MaterialType& id)
{
	if (const auto iter = m_Shaders.find(id); iter != m_Shaders.end())
	{
		return iter->second;
	}

	return nullptr;
}

void Resource::CreateShaders(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, const UINT cbvSrvDescriptorSize)
{
	std::wstring shaderPathSimple = SHADER_ROOT + L"Simple.hlsl";
	m_Shaders[SIMPLE] = new VGShaderColor(device, cbvHeap, cbvSrvDescriptorSize, shaderPathSimple);
	m_Shaders[SIMPLE]->Init();

	std::wstring shaderPathTex = SHADER_ROOT + L"Texture.hlsl";
	m_Shaders[TEXTURE] = new VGShaderTexture(device, cbvHeap, cbvSrvDescriptorSize, shaderPathTex);
	m_Shaders[TEXTURE]->Init();

	std::wstring shaderPathTexTrans = SHADER_ROOT + L"Texture_UI.hlsl";
	m_Shaders[TEXTURE_UI] = new VGShaderTextureUI(device, cbvHeap, cbvSrvDescriptorSize, shaderPathTexTrans);
	m_Shaders[TEXTURE_UI]->Init();

	std::wstring shaderPathParticle = SHADER_ROOT + L"Particle.hlsl";
	m_Shaders[PARTICLE] = new VGShaderParticle(device, cbvHeap, cbvSrvDescriptorSize, shaderPathParticle);
	m_Shaders[PARTICLE]->Init();

	std::wstring shaderPathSkybox = SHADER_ROOT + L"Sky.hlsl";
	m_Shaders[SKYBOX] = new VGShaderSkybox(device, cbvHeap, cbvSrvDescriptorSize, shaderPathSkybox);
	m_Shaders[SKYBOX]->Init();
}

void Resource::CreateMaterials()
{
	CreateMaterial(SIMPLE, "Simple");
	CreateMaterial(TEXTURE, "Texture");
	CreateMaterial(TEXTURE_UI, "TextureOffset");
	CreateMaterial(PARTICLE, "Particle");
	CreateMaterial(SKYBOX, "Skybox");
}

void Resource::CreateMaterial(const MaterialType& mt, const std::string& name)
{
	m_Materials[mt] = new VGMaterial();
	m_Materials[mt]->SetShader(m_Shaders[mt]);
	m_Materials[mt]->Name = name;
}

int Resource::AddToResourceHeap(IResourceObject* resObj, int resType)
{
	if (!resObj) return -1;

	// Everytime we create a new texture, we need to store it in the m_pCbvSrvHeap.
	// To do so, we create a new descriptor handle and offset it by the number of textures already stored in the heap.
	// The methods returns the index of the texture in the heap, which will be stored in the Texture class.
	ID3D12DescriptorHeap* cbvSrvHeap = nullptr;
	UINT heapSize = I(VGHandler)->GetCbvHeap(&cbvSrvHeap);
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(cbvSrvHeap->GetCPUDescriptorHandleForHeapStart());
	hDescriptor.Offset(m_TexIndex, heapSize);

	ID3D12Resource* res = resObj->GetResource();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = static_cast<D3D12_SRV_DIMENSION>(resType);
	srvDesc.Format = res->GetDesc().Format;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = res->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	I(VGHandler)->GetDevice()->CreateShaderResourceView(res, &srvDesc, hDescriptor);

	return m_TexIndex++;
}
