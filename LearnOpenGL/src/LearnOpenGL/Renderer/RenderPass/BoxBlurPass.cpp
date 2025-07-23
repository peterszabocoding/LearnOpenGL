#include "pch.h"
#include "RenderPass.h"
#include "LearnOpenGL/Renderer/Framebuffer.h"
#include "LearnOpenGL/Renderer/MeshPrimitives.h"
#include "LearnOpenGL/Renderer/ShaderManager.h"

namespace LearnOpenGL
{
	void BoxBlurPass::Render(Ref<Framebuffer> targetBuffer, uint32_t source, char amount, float blurIntensity)
	{
		targetBuffer->Bind();
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::POST_PROCESS_BOX_BLUR);
		shader->Bind();
		shader->BindTexture(0, source);
		shader->SetFloat("blurIntensity", blurIntensity);

		for (int i = 0; i < amount; i++) RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);

		shader->Unbind();
		targetBuffer->Unbind();
	}
}
