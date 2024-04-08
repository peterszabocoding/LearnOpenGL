#include "mgpch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"

namespace Moongoose {

	void Renderer::BeginScene() {}

	void Renderer::EndScene() {}

	void Renderer::RenderMesh(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

	/*
	void Renderer::RenderMesh(Shader* shader, Material* material, const Ref<VertexArray>& vertexArray, const glm::mat4& modelMatrix)
	{
		shader->Bind();
		shader->SetModelTransform(modelMatrix);

		if (material && material->GetUUID()) {
			const auto& albedo = material->GetAlbedo();
			const auto& normal = material->GetNormal();
			const auto& roughness = material->GetRoughness();
			const auto& metallic = material->GetMetallic();

			if (albedo) shader->BindTexture(0, albedo->GetTextureID());
			if (normal) shader->BindTexture(1, normal->GetTextureID());
			if (roughness) shader->BindTexture(2, roughness->GetTextureID());
			if (metallic) shader->BindTexture(3, metallic->GetTextureID());

			vertexArray->Bind();

			RenderCommand::DrawIndexed(vertexArray);

			vertexArray->Unbind();
		}
		else {
			LOG_CORE_ERROR("Missing material!");
		}

		shader->Unbind();
	}
	*/

};
