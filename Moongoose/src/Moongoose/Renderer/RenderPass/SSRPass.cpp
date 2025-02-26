#include "mgpch.h"
#include "RenderPass.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"
#include "Moongoose/Renderer/GBuffer.h"

namespace Moongoose
{
	void PostProcessingSSRPass::Render(Ref<Framebuffer> targetBuffer, RenderPassParams& renderPassParams)
	{
		const PostProcessingSSRPassData* data = static_cast<PostProcessingSSRPassData*>(renderPassParams.
			additionalData);
		const auto resolution = renderPassParams.camera->GetResolution();
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::POST_PROCESS_SSR);

		targetBuffer->Bind();
		RenderCommand::SetClearColor(targetBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		shader->Bind();
		shader->DisableFeature(GlFeature::DEPTH_TEST);
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
		shader->SetInt("steps", data->ssrParams.steps);

		RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);

		shader->EnableFeature(GlFeature::DEPTH_TEST);
		shader->Unbind();
		targetBuffer->Unbind();
	}
}
