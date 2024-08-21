#include "mgpch.h"
#include "RenderPass.h"
#include "Moongoose/Renderer/FramebufferManager.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose
{
	void SSRPass::Render(void* renderParams)
	{
		const auto params = static_cast<Params*>(renderParams);
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::POST_PROCESS_SSR);

		if (!framebuffer) InitFramebuffer(params->resolution);

		framebuffer->Bind();
		RenderCommand::SetClearColor(framebuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		shader->Bind();
		shader->SetCamera(
			params->camera->GetCameraPosition(),
			params->camera->GetViewMatrix(),
			params->camera->GetProjection());

		shader->BindTexture(0, params->renderTexture);
		shader->BindTexture(1, params->gBuffer->GetViewPositionTexture());
		shader->BindTexture(2, params->gBuffer->GetNormalTexture());
		shader->BindTexture(3, params->gBuffer->GetRoughnessTexture());
		shader->BindTexture(4, params->gBuffer->GetDepthAttachment());

		RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);

		shader->Unbind();
		framebuffer->Unbind();
	}

	void SSRPass::InitFramebuffer(const glm::uvec2 resolution)
	{
		FramebufferSpecs specs;
		specs.width = resolution.x;
		specs.height = resolution.y;
		specs.attachments = {
			FramebufferTextureFormat::RGBA8
		};

		framebuffer = FramebufferManager::CreateFramebuffer("SSRBuffer");
		framebuffer->Configure(specs);
	}
}
