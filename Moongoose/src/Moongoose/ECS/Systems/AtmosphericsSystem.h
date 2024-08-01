#pragma once

#include "Moongoose/ECS/WorldManager.h"
#include "Moongoose/ECS/SystemManager.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"
#include "Moongoose/Renderer/Framebuffer.h"

namespace Moongoose {

	class AtmosphericsSystem : public System
	{
	public:
		AtmosphericsSystem();
		~AtmosphericsSystem();

		virtual Signature GetSystemSignature(World* world);

		void Init();
		void Update(const Ref<PerspectiveCamera>& camera, glm::vec2 resolution);
		void Run(const Ref<PerspectiveCamera>& camera, Ref<World> world);


	private:
		bool isLUTGenerated = false;
		Ref<Framebuffer> m_LUTBuffer;
		Ref<Framebuffer> m_TransmittanceBuffer;
		Ref<Framebuffer> m_MultiScatteringBuffer;
		Ref<Framebuffer> m_RaymarchingBuffer;
		Ref<Framebuffer> m_SkyBuffer;
		 
		Ref<Shader> m_TransmittanceShader;
		Ref<Shader> m_MultiScatteringShader;
		Ref<Shader> m_RaymarchingShader;
		Ref<Shader> m_SkyShader;

	};
}

