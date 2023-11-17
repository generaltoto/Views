#pragma once 

#include "VGMesh.h"

/*
Static class that handles the creation and destruction of all meshes used in the engine.
This meshes are only used as a buffer, each Renderer uses the meh without modifying it.
This saves us a lot of memory to prevent creating the same mesh multiple times.
*/
class VGGeometryHandler final
{
public:
	static void CreateAllMeshes();
	static void DestroyAllMeshes();

	static VGMesh* GetMesh(const MeshType type) { return m_Meshes[type]; }

private:

	static VGMesh* CreateCube();

	static VGMesh* CreateUVSphere();
	static VGMesh* CreateGeoSphere();
	static void Subdivide(std::vector<VGVertex>& vertices, std::vector<UINT>& indices);
	static VGVertex MidPoint(const VGVertex& v0, const VGVertex& v1);

	static VGMesh* CreatePyramid();

	static VGMesh* CreateUISquare(const float uvOffsetX, const float uvOffsetY);

private:

	static VGMesh* m_Meshes[5];
};