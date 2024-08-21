#pragma once

#include <glm/glm.hpp>

#include "Light.h"
#include "GBuffer.h"
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
		};

		static void RenderWorld(const Ref<PerspectiveCamera>& camera, const Ref<World>& world);
		static void RenderMesh(const Ref<VertexArray>& vertexArray);

		static void PushMeshRenderCommand(const MeshCommand& cmd);
		static void PushBillboardRenderCommand(const BillboardCommand& cmd);

		static void PushDirectionalLight(const DirectionalLight& light);
		static void PushPointLight(const PointLight& light);
		static void PushSpotLight(const SpotLight& light);

		static unsigned int GetDrawCount() { return prevDrawCount; }
		static Ref<Framebuffer> GetRenderBuffer() { return m_RenderBuffer; }

	private:
		static void InitShadowBuffer();
		static void InitRenderBuffer();

		static void SetResolution(glm::uvec2 newResolution);

		static void SetLights();
		static void AddDirectionalLight(const DirectionalLight& light, const Ref<Shader>& shader);

		static void RenderShadowMaps();

		static void ExecuteMeshRenderCommand(const Ref<PerspectiveCamera>& camera, const MeshCommand& cmd);
		static void ExecuteBillboardRenderCommand(const Ref<PerspectiveCamera>& camera, const BillboardCommand& cmd);

		static void AddSpotLight(size_t index, const Ref<Shader>& shader, const SpotLight& spotLight);
		static void AddPointLight(size_t index, const Ref<Shader>& shader, const PointLight& pointLight);

	private:
		static constexpr glm::uvec2 SHADOW_BUFFER_RESOLUTION = {4096, 4096};
		static constexpr unsigned int CUBE_SHADOW_MAP_RESOLUTION = 512;

		static glm::uvec2 m_Resolution;

		static std::vector<DirectionalLight> m_DirectionalLights;
		static std::vector<PointLight> m_PointLights;
		static std::vector<SpotLight> m_SpotLights;

		static std::vector<MeshCommand> m_MeshRenderCmds;
		static std::vector<BillboardCommand> m_BillboardRenderCmds;

		static Ref<Framebuffer> m_RenderBuffer;

		static TextureAtlas m_TextureAtlas;

		static unsigned int prevDrawCount;
		static unsigned int currentDrawCount;

		static SSRPass m_SsrPass;
		static GeometryPass m_GeometryPass;
		static ShadowMapPass m_ShadowMapPass;
	};
}
