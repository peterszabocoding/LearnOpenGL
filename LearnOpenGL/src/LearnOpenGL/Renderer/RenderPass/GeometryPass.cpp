#include "pch.h"
#include "RenderPass.h"
#include "LearnOpenGL/Renderer/RenderCommand.h"
#include "LearnOpenGL/Renderer/ShaderManager.h"

#include "LearnOpenGL/Renderer/GBuffer.h"

namespace LearnOpenGL
{
	void GeometryPass::Render(Ref<GBuffer> gBuffer, const RenderPassParams& renderPassParams)
	{
		const auto shader = ShaderManager::GetShaderByType(ShaderType::GBUFFER);

		gBuffer->Bind();
		RenderCommand::SetClearColor(gBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		shader->Bind();
		shader->SetCamera(
			renderPassParams.camera->GetCameraPosition(),
			renderPassParams.camera->GetViewMatrix(),
			renderPassParams.camera->GetProjection());

		for (const auto& [id, transform, vertexArray, material] : renderPassParams.meshCommands)
		{
			if (material->m_Normal) material->m_Normal->Bind(0);
			if (material->m_Roughness) material->m_Roughness->Bind(1);

			shader->SetInt("aEntityID", id);
			shader->SetMat4("model", transform);
			shader->SetFloat("roughnessValue", material->m_RoughnessValue);
			shader->SetInt("bUseNormalMap", material->m_Normal != nullptr ? 1 : 0);
			shader->SetInt("bUseRoughnessMap", material->m_Roughness != nullptr ? 1 : 0);

			RenderCommand::DrawIndexed(vertexArray);
		}

		shader->Unbind();
		gBuffer->Unbind();
	}
}
