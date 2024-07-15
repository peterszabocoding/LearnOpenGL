#pragma once

#include "Moongoose/Core.h"
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

		static Ref<World> CreateWorld(const std::string& name) { return Get().createWorld(name); }
		static Ref<World> LoadWorld(const std::string& sceneFile) { return Get().loadWorld(sceneFile); }
		static Ref<World> GetLoadedWorld() { return Get().getLoadedWorld(); }
		static void SaveWorld(const std::string& sceneFile) { return Get().saveWorld(sceneFile); };
		static bool IsWorldOpened() { return Get().isWorldOpened(); }
		static void CloseWorld(Ref<World> world) { return Get().closeWorld(world); }

		Ref<World> createWorld(const std::string& name);
		Ref<World> loadWorld(const std::string& sceneFile);
		const Ref<World>& getLoadedWorld() const { return m_LoadedWorld; }
		bool isWorldOpened() const { return m_LoadedWorld != nullptr; }
		void saveWorld(const std::string& sceneFile) const;
		void closeWorld(Ref<World> world);

	private:
		WorldManager() {}
		~WorldManager() = default;

	private:
		Ref<World> m_LoadedWorld;
		Ref<World> m_InspectorWorld;
	};

}

