#include "mgpch.h"
#include "Renderer.h"

#include "FramebufferManager.h"
#include "RenderCommand.h"
#include "Moongoose/ECS/World.h"
#include "Moongoose/ECS/Systems/AtmosphericsSystem.h"
#include "Moongoose/ECS/Systems/BillboardSystem.h"
#include "Moongoose/ECS/Systems/LightSystem.h"
#include "Moongoose/ECS/Systems/RenderSystem.h"
#include "Moongoose/Renderer/Framebuffer.h"

namespace Moongoose {

	glm::uvec2 Renderer::m_Resolution;
	Ref<Framebuffer> Renderer::m_RenderBuffer;

	void Renderer::SetResolution(const glm::uvec2 resolution)
	{
		FramebufferSpecs specs;
		specs.Width = resolution.x;
		specs.Height = resolution.y;
		specs.Attachments = {
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::DEPTH24STENCIL8
		};
		specs.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		m_RenderBuffer = FramebufferManager::CreateFramebuffer("RenderBuffer", specs);
		m_Resolution = resolution;
	}

	void Renderer::RenderWorld(const Ref<PerspectiveCamera>& camera, const Ref<World>& world)
	{
		world->GetSystem<AtmosphericsSystem>()->Update(camera, m_Resolution);

		m_RenderBuffer->Bind();

		RenderCommand::SetClearColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f });
		RenderCommand::Clear();

		world->GetSystem<AtmosphericsSystem>()->Run(camera, world);
		world->GetSystem<LightSystem>()->Run(camera, world);
		m_RenderBuffer->ClearAttachment(1, -1);
		world->GetSystem<RenderSystem>()->Run(camera, world);
		world->GetSystem<BillboardSystem>()->Run(camera, world);

		m_RenderBuffer->Unbind();
	}

	void Renderer::BeginScene() {}

	void Renderer::EndScene() {}

	void Renderer::RenderMesh(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

};
