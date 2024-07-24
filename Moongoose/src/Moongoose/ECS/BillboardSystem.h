#pragma once

#include "WorldManager.h"
#include "SystemManager.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"

namespace Moongoose {

	class BillboardSystem : public System
	{
	public:
		BillboardSystem()
		{
			m_DefaultMaterial = CreateRef<Material>("DefaultMaterial");
		}

		virtual Signature GetSystemSignature(World* world);

		void Run(const Ref<PerspectiveCamera>& camera, Ref<World> world);

	private:
		Ref<Material> m_DefaultMaterial;
	};
}

