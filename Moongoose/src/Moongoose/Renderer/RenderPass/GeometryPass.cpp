#include "mgpch.h"
#include "RenderPass.h"
#include "Moongoose/Renderer/RenderCommand.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose
{
	void GeometryPass::Render(void* renderSpecs)
	{
		const auto specs = static_cast<Specs*>(renderSpecs);
		const auto shader = ShaderManager::GetShaderByType(ShaderType::GBUFFER);

		if (!m_GBuffer)
			m_GBuffer = CreateRef<GBuffer>(GBuffer::GBufferSpecs({specs->resolution.x, specs->resolution.y}));
		else
			m_GBuffer->Resize(specs->resolution.x, specs->resolution.y);


		m_GBuffer->Bind();
		RenderCommand::SetClearColor(m_GBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		shader->Bind();

		for (const MeshCommand& cmd : specs->meshCommands)
		{
			shader->SetCamera(
				specs->camera->GetCameraPosition(),
				specs->camera->GetViewMatrix(),
				specs->camera->GetProjection());

			if (cmd.material->m_Normal) cmd.material->m_Normal->Bind(0);
			if (cmd.material->m_Roughness) cmd.material->m_Roughness->Bind(1);

			shader->UploadUniformInt("bUseNormalMap", cmd.material->m_Normal != nullptr ? 1 : 0);
			shader->UploadUniformInt("bUseRoughnessMap", cmd.material->m_Roughness != nullptr ? 1 : 0);

			shader->SetFloat("roughnessValue", cmd.material->m_RoughnessValue);

			shader->SetMat4("model", cmd.transform);
			RenderCommand::DrawIndexed(cmd.vertexArray);
		}

		shader->Unbind();

		m_GBuffer->Unbind();
	}
}
