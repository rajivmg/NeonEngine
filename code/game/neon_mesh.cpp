#include "neon_mesh.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "neon_renderer.h"

void PushMesh(std::vector<vert_P1C1UV1> *Vertices, std::vector<vert_index> *Indices, char const *File, int MeshIndex)
{
	const aiScene *Scene = aiImportFile(File, aiProcess_Triangulate);

	if(!Scene)
	{
		Platform.Log("Mesh loading error: %s", aiGetErrorString());
		return;
	}

	// Process
	aiMesh const *Mesh = Scene->mMeshes[MeshIndex];
	for(u32 I = 0; I < Mesh->mNumVertices; ++I)
	{
		vert_P1C1UV1 Vertex;
		Vertex.Position = vec3(Mesh->mVertices[I].x, Mesh->mVertices[I].y, Mesh->mVertices[I].z);
		Vertex.UV = Mesh->HasTextureCoords(0) ? vec2(Mesh->mTextureCoords[0][I].x, Mesh->mTextureCoords[0][I].y) : vec2i(0, 0);
		Vertex.Color = Mesh->HasVertexColors(0) ? vec4(Mesh->mColors[0][I].r, Mesh->mColors[0][I].g, Mesh->mColors[0][I].b, Mesh->mColors[0][I].a) : vec4(0.7f, 0.7f, 0.7f, 1.0f);
		Vertices->push_back(Vertex);
	}

	for(u32 J = 0; J < Mesh->mNumFaces; ++J)
	{
		aiFace *Face = &Mesh->mFaces[J];
		assert(Face->mNumIndices == 3);
		Indices->push_back(Face->mIndices[0]);
		Indices->push_back(Face->mIndices[1]);
		Indices->push_back(Face->mIndices[2]);
	}
	// End Process

	aiReleaseImport(Scene);
}

void PushMesh(std::vector<vert_P1N1UV1> *Vertices, std::vector<vert_index> *Indices, char const *File, int MeshIndex)
{
	aiScene const *Scene = aiImportFile(File, aiProcess_Triangulate);

	if(!Scene)
	{
		Platform.Log("Mesh loading error: %s", aiGetErrorString());
		return;
	}

	aiMesh const *Mesh = Scene->mMeshes[MeshIndex];
	for(u32 I = 0; I < Mesh->mNumVertices; ++I)
	{
		vert_P1N1UV1 Vertex;
		Vertex.Position = vec3(Mesh->mVertices[I].x, Mesh->mVertices[I].y, Mesh->mVertices[I].z);
		Vertex.Normal = Mesh->HasNormals() ? vec3(Mesh->mNormals[I].x, Mesh->mNormals[I].y, Mesh->mNormals[I].z) : vec3i(1, 1, 1);
		Vertex.UV = Mesh->HasTextureCoords(0) ? vec2(Mesh->mTextureCoords[0][I].x, Mesh->mTextureCoords[0][I].y) : vec2i(0, 0);
		Vertices->push_back(Vertex);
	}
	for(u32 J = 0; J < Mesh->mNumFaces; ++J)
	{
		aiFace *Face = &Mesh->mFaces[J];
		assert(Face->mNumIndices == 3);
		Indices->push_back(Face->mIndices[0]);
		Indices->push_back(Face->mIndices[1]);
		Indices->push_back(Face->mIndices[2]);
	}

	aiReleaseImport(Scene);
}