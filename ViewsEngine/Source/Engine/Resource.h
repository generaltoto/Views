#pragma once

#include <unordered_map>

class VGShaderBase;
class VGMaterial;
class IResourceObject;
struct ID3D12Device;
struct ID3D12DescriptorHeap;

class Resource
{
public:
	Resource() = default;
	~Resource();

	template <class T = IResourceObject>
	static T* Load(const char* filepath)
	{
		std::string path = filepath;

		std::string name = path.substr(path.find_last_of("/") + 1, path.find_last_of("."));

		if (m_Resources.contains(name))
		{
			IResourceObject* resource = m_Resources[name];
			T* cRes = reinterpret_cast<T*>(resource);
			return cRes;
		}

		T* resource = new T(name);
		resource->Load(path);
		m_Resources[name] = resource;

		AddToResourceHeap(resource, resource->GetResType());

		return resource;
	}

	static VGMaterial* LoadMaterial(MaterialType matType);

	static void CreateResources(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvSrvDescriptorSize);
	static void ReleaseResources();
	static int AddToResourceHeap(IResourceObject* resObj, int resType);

	static std::unordered_map<MaterialType, VGShaderBase*>& GetShaders() { return m_Shaders; }
	static std::unordered_map<std::string, IResourceObject*>& GetResources() { return m_Resources; }
	static IResourceObject* GetResource(const std::string& name) { return m_Resources[name]; }

private:
	static std::unordered_map<MaterialType, VGShaderBase*> m_Shaders;
	static std::unordered_map<MaterialType, VGMaterial*> m_Materials;
	static std::unordered_map<std::string, IResourceObject*> m_Resources;

	static int m_TexIndex;

	static VGShaderBase* FindShader(const MaterialType& id);

	static void CreateShaders(ID3D12Device* device, ID3D12DescriptorHeap* cbvHeap, UINT cbvSrvDescriptorSize);

	static void CreateMaterials();
	static void CreateMaterial(const MaterialType& mt, const std::string& name);

	inline static const std::wstring SHADER_ROOT = L"Resources/Shaders/";

};