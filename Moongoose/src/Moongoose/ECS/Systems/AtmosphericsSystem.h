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
		~AtmosphericsSystem() override = default;

		virtual Signature GetSystemSignature(World* world) override;

		void Init() const;
		void Update(const Ref<PerspectiveCamera>& camera, glm::vec2 resolution) const;
		void Run(const Ref<PerspectiveCamera>& camera) const;


	private:
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

