#pragma once

#include "Moongoose/Core.h"
#include "Entity.h"

namespace Moongoose {

	class Scene
	{
	public:
		Scene() {}
		~Scene() {}

		Entity AddEntity(std::string name);
		std::vector<Entity>& getEntities();


	private:
		std::vector<Entity> m_Entities;
	};

}

