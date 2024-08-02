#pragma once

#include "Moongoose/ECS/WorldManager.h"
#include "Moongoose/ECS/SystemManager.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"

namespace Moongoose {

	class RenderSystem: public System
	{
	public:
		RenderSystem()
		{
			m_DefaultMaterial = CreateRef<Material>("DefaultMaterial");
		}

		virtual Signature GetSystemSignature(World* world) override;

		void Run(const Ref<PerspectiveCamera>& camera, const Ref<World>& world) const;

	private:
		Ref<Material> m_DefaultMaterial;
	};
}

