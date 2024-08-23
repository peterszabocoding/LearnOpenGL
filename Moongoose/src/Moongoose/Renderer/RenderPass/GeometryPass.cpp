#include "mgpch.h"
#include "RenderPass.h"
#include "Moongoose/Renderer/RenderCommand.h"
#include "Moongoose/Renderer/ShaderManager.h"

#include "Moongoose/Renderer/GBuffer.h"

namespace Moongoose
{
	void GeometryPass::Render(const RenderPassParams& renderPassParams)
	{
		const auto shader = ShaderManager::GetShaderByType(ShaderType::GBUFFER);
		const auto resolution = renderPassParams.camera->GetResolution();

		if (!m_GBuffer)
			m_GBuffer = CreateRef<GBuffer>(GBuffer::GBufferSpecs({resolution.x, resolution.y}));
		else
			m_GBuffer->Resize(resolution.x, resolution.y);

		m_GBuffer->Bind();
		RenderCommand::SetClearColor(m_GBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		shader->Bind();
		shader->SetCamera(
			renderPassParams.camera->GetCameraPosition(),
			renderPassParams.camera->GetViewMatrix(),
			renderPassParams.camera->GetProjection());

		for (const MeshCommand& cmd : renderPassParams.meshCommands)
		{
			if (cmd.material->m_Normal) cmd.material->m_Normal->Bind(0);
			if (cmd.material->m_Roughness) cmd.material->m_Roughness->Bind(1);

			shader->SetMat4("model", cmd.transform);
			shader->UploadUniformInt("bUseNormalMap", cmd.material->m_Normal != nullptr ? 1 : 0);
			shader->UploadUniformInt("bUseRoughnessMap", cmd.material->m_Roughness != nullptr ? 1 : 0);
			shader->SetFloat("roughnessValue", cmd.material->m_RoughnessValue);

			RenderCommand::DrawIndexed(cmd.vertexArray);
		}

		shader->Unbind();
		m_GBuffer->Unbind();
	}

	void GeometryPass::Resize(const glm::uvec2& resolution)
	{
		if (!m_GBuffer) return;
		m_GBuffer->Resize(resolution.x, resolution.y);
	}
}
