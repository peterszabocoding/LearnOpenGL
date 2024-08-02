#pragma once

#include "Moongoose/ECS/SystemManager.h"
#include "Moongoose/ECS/Systems/RenderSystem.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"

namespace Moongoose {

	class BillboardSystem : public System
	{
	public:
		BillboardSystem();

		virtual Signature GetSystemSignature(World* world) override;

		void Run(const Ref<PerspectiveCamera>& camera, const Ref<World>& world) const;

	private:
		Ref<Material> m_DefaultMaterial;
	};
}

