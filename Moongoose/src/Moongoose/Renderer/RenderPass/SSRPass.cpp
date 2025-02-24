#include "mgpch.h"
#include "RenderPass.h"
#include "Moongoose/Renderer/FramebufferManager.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"

#include "Moongoose/Renderer/GBuffer.h"

namespace Moongoose
{
	void SsrPass::Render(const RenderPassParams& renderPassParams)
	{
		const SsrPassData* data = static_cast<SsrPassData*>(renderPassParams.additionalData);
		const auto resolution = renderPassParams.camera->GetResolution();
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::POST_PROCESS_SSR);

		if (!framebuffer) InitFramebuffer(resolution);

		framebuffer->Bind();
		RenderCommand::SetClearColor(framebuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		shader->Bind();
		shader->SetCamera(
			renderPassParams.camera->GetCameraPosition(),
			renderPassParams.camera->GetViewMatrix(),
			renderPassParams.camera->GetProjection());

		shader->BindTexture(0, data->renderTexture);
		shader->BindTexture(1, data->gBuffer->GetViewPositionTexture());
		shader->BindTexture(2, data->gBuffer->GetNormalTexture());
		shader->BindTexture(3, data->gBuffer->GetRoughnessTexture());
		shader->BindTexture(4, data->gBuffer->GetDepthAttachment());

		shader->SetFloat("maxDistance", data->ssrParams.maxDistance);
		shader->SetFloat("resolution", data->ssrParams.resolution);
		shader->SetFloat("thickness", data->ssrParams.thickness);
		shader->UploadUniformInt("steps", data->ssrParams.steps);

		RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);

		shader->Unbind();
		framebuffer->Unbind();
	}

	void SsrPass::Resize(const glm::uvec2& resolution)
	{
		if (!framebuffer) return;
		framebuffer->Resize(resolution.x, resolution.y);
	}

	void SsrPass::InitFramebuffer(const glm::uvec2 resolution)
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
