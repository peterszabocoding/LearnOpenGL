#pragma once

#include "Moongoose/Core.h"
#include "Entity.h"

namespace Moongoose {

	class Scene
	{
	public:
		Scene() {}
		~Scene() {}

		Ref<Entity> AddEntity(std::string name);
		std::vector<Ref<Entity>>& getEntities();


	private:
		std::vector<Ref<Entity>> m_Entities;
	};

}

