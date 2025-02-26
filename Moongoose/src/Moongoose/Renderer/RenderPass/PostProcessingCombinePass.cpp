#include "mgpch.h"
#include "RenderPass.h"
#include "Moongoose/Renderer/Framebuffer.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose
{
	void PostProcessCombinePass::Render(const Ref<Framebuffer> targetBuffer, uint32_t baseTexture,
	                                    uint32_t textureToBlend, float multiply)
	{
		targetBuffer->Bind();
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::POST_PROCESS_COMBINE);
		shader->Bind();
		shader->BindTexture(0, baseTexture);
		shader->BindTexture(1, textureToBlend);
		shader->SetFloat("multiply", multiply);

		RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);

		shader->Unbind();
		targetBuffer->Unbind();
	}
}
