#include "neon_mesh.h"

static
void ProcessScene(aiScene const * Scene)
{

}

void MeshFromFile(char const * Filename)
{
	const aiScene *Scene = aiImportFile(Filename, 
										aiProcess_Triangulate |
										aiProcess_JoinIdenticalVertices |
										aiProcess_SortByPType |
										aiProcess_FlipWindingOrder);
	if(!Scene)
	{
		Platform->Log(ERR, "Mesh_Import:%s", aiGetErrorString());
	} 

	ProcessScene(Scene);

	// release the resources associated with the import
	aiReleaseImport(Scene);
}