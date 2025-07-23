#include "pch.h"
#include "RenderPass.h"
#include "LearnOpenGL/Renderer/Framebuffer.h"
#include "LearnOpenGL/Renderer/MeshPrimitives.h"
#include "LearnOpenGL/Renderer/ShaderManager.h"

namespace LearnOpenGL
{
	void BillboardPass::Render(Ref<Framebuffer> targetBuffer, RenderPassParams& renderPassParams)
	{
		const auto data = static_cast<BillboardPassData*>(renderPassParams.additionalData);
		if (data->billboardCommands.empty()) return;

		targetBuffer->Bind();

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
			shader->SetInt("aEntityID", id);
			shader->SetFloat3("TintColor", tintColor);

			texture->Bind(0);

			RenderCommand::DrawIndexed(QuadMeshWorld(scale).GetSubmeshes()[0]->vertexArray);
		}

		shader->DisableFeature(GlFeature::BLEND);
		shader->Unbind();

		targetBuffer->Unbind();
	}
}
