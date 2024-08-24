#include "mgpch.h"
#include "RenderPass.h"
#include "Moongoose/Renderer/Framebuffer.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose
{
	void BoxBlurPass::Render(const RenderPassParams& renderPassParams)
	{
		const auto data = static_cast<BoxBlurPassData*>(renderPassParams.additionalData);

		for (int i = 0; i < 2; i++)
		{
			data->targetBuffer->Bind();

			const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::POST_PROCESS_BOX_BLUR);
			shader->Bind();
			shader->BindTexture(0, data->colorTexture);

			RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);

			shader->Unbind();
			data->targetBuffer->Unbind();
		}
	}
}
