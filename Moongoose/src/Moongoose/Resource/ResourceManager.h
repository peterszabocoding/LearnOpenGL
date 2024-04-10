#pragma once

#include "Moongoose/Core.h"

class aiMesh;
class aiScene;
class aiNode;
class Mesh;

namespace Moongoose {

	class ResourceManager
	{
	public:
		static Mesh* LoadMesh(std::string meshPath);

	private:
		static Mesh* LoadMesh(aiMesh* mesh, const aiScene* scene);
		static void LoadNode(aiNode* node, const aiScene* scene);
	};

}