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

		Ref<World> CreateWorld(const std::string& name);
		Ref<World> LoadWorld(const std::string& sceneFile);
		void SaveWorld(const std::string& sceneFile) const;
		void CloseWorld(Ref<World> world);

		const Ref<World>& GetLoadedWorld() const { return m_LoadedWorld; }

	private:
		WorldManager() {}
		~WorldManager() = default;

	private:
		Ref<World> m_LoadedWorld;
		Ref<World> m_InspectorWorld;

	};

}

