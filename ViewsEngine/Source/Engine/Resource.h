#pragma once

#include <unordered_map>
#include "D3D/Shaders/Base/VGMaterial.h"

class IResourceObject;
struct ID3D12Device;
struct ID3D12DescriptorHeap;

class Resource
{
public:
	template <class R> requires std::derived_from<R, IResourceObject>
	[[nodiscard]]
	static R* Load(const char* filepath)
	{
		std::string path = filepath;

		std::string name = path.substr(path.find_last_of("/") + 1, path.find_last_of("."));

		if (m_Resources.contains(name))
		{
			IResourceObject* resource = m_Resources[name];
			R* cRes = reinterpret_cast<R*>(resource);
			return cRes;
		}

		R* resource = new R(name);
		resource->Load(path);
		m_Resources[name] = resource;

		AddToResourceHeap(resource, resource->GetResType());

		return resource;
	}

	static VGMaterial* LoadMaterial(MaterialType matType);

	static void CreateResources(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvSrvDescriptorSize);
	static void ReleaseResources();
	static int AddToResourceHeap(IResourceObject* resObj, int resType);

	static std::unordered_map<MaterialType, VGShader*>& GetShaders() { return m_Shaders; }
	static std::unordered_map<std::string, IResourceObject*>& GetResources() { return m_Resources; }
	static IResourceObject* GetResource(const std::string& name);

private:
	Resource() = default;
	~Resource() = default;
	
	static std::unordered_map<MaterialType, VGShader*> m_Shaders;
	static std::unordered_map<std::string, IResourceObject*> m_Resources;

	static int m_TexIndex;

	static VGShader* FindShader(const MaterialType& id);
	static void CreateShaders(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvSrvDescriptorSize);

	static void CreateMaterials();
	static void CreateMaterial(const MaterialType& mt, const std::string& name);

	inline static const std::wstring SHADER_ROOT = L"Resources/Shaders/";

};