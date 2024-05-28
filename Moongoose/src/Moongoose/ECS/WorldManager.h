#pragma once

#include "Moongoose/Core.h"
#include "Entity.h"
#include "World.h"

namespace Moongoose {

	class WorldManager
	{
	public:
		static WorldManager& Get()
		{
			static WorldManager manager;
			return manager;
		}

		Ref<World> CreateWorld(const std::string& name)
		{
			if (m_LoadedWorld) CloseWorld(m_LoadedWorld);

			m_LoadedWorld = CreateRef<World>();
			m_LoadedWorld->Init();

			m_LoadedWorld->m_ID = UUID();
			m_LoadedWorld->m_Name = name;

			return m_LoadedWorld;
		}

		void CloseWorld(Ref<World> world)
		{

		}

		Ref<World> LoadWorld(const std::string& sceneFile)
		{

		}

		void SaveWorld(const std::string& sceneFile, Ref<World> scene)
		{

		}

		const Ref<World>& GetLoadedWorld() const { return m_LoadedWorld; }

	private:
		WorldManager() {};
		~WorldManager() = default;

	private:
		Ref<World> m_LoadedWorld;
		Ref<World> m_InspectorWorld;

	};

}

