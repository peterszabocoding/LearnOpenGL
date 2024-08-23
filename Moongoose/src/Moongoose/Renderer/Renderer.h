#pragma once

#include <glm/glm.hpp>

#include "Light.h"
#include "VertexArray.h"
#include "TextureAtlas.h"
#include "RenderCommand.h"
#include "Moongoose/Core.h"
#include "PerspectiveCamera.h"
#include "RenderPass/RenderPass.h"

namespace Moongoose
{
	class Framebuffer;
	class World;

	class Renderer
	{
	public:
		struct AtlasBox
		{
			glm::uvec2 topLeft;
			glm::uvec2 bottomRight;
		};

	public:
		static void BeginScene();

		static void EndScene()
		{
		}

		static void RenderWorld(const Ref<PerspectiveCamera>& camera, const Ref<World>& world);
		static void RenderMesh(const Ref<VertexArray>& vertexArray);

		static unsigned int GetDrawCount() { return prevDrawCount; }
		static Ref<Framebuffer> GetRenderBuffer() { return m_LightingPass.GetFramebuffer(); }

	private:
		static void SetResolution(glm::uvec2 newResolution);

	private:
		static constexpr glm::uvec2 SHADOW_BUFFER_RESOLUTION = {4096, 4096};
		static constexpr unsigned int CUBE_SHADOW_MAP_RESOLUTION = 512;

		static glm::uvec2 m_Resolution;

		static std::vector<DirectionalLight> m_DirectionalLights;
		static std::vector<PointLight> m_PointLights;
		static std::vector<SpotLight> m_SpotLights;

		static std::vector<MeshCommand> m_MeshRenderCmds;
		static std::vector<BillboardCommand> m_BillboardRenderCmds;

		static unsigned int prevDrawCount;

		static SsrPass m_SsrPass;
		static LightingPass m_LightingPass;
		static GeometryPass m_GeometryPass;
		static ShadowMapPass m_ShadowMapPass;
		static BillboardPass m_BillboardPass;
	};
}
