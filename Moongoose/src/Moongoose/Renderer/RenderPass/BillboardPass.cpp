#include "mgpch.h"
#include "RenderPass.h"
#include "Moongoose/Renderer/Framebuffer.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose
{
	void BillboardPass::Render(const RenderPassParams& renderPassParams)
	{
		const auto data = static_cast<BillboardPassData*>(renderPassParams.additionalData);
		if (data->billboardCommands.empty()) return;

		data->targetBuffer->Bind();

		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::BILLBOARD);
		shader->Bind();
		shader->EnableFeature(GlFeature::BLEND);
		shader->SetBlendMode(GlBlendOption::SRC_ALPHA, GlBlendOption::ONE_MINUS_SRC_ALPHA);
		shader->SetCamera(
			renderPassParams.camera->GetCameraPosition(),
			renderPassParams.camera->GetViewMatrix(),
			renderPassParams.camera->GetProjection());

		for (auto [id, transform, texture, scale, tintColor] : data->billboardCommands)
		{
			shader->SetMat4("model", transform);
			shader->UploadUniformInt("aEntityID", id);
			shader->SetFloat3("TintColor", tintColor);

			texture->Bind(0);

			RenderCommand::DrawIndexed(QuadMeshWorld(scale).GetSubmeshes()[0]->vertexArray);
		}

		shader->DisableFeature(GlFeature::BLEND);
		shader->Unbind();
	}
}
