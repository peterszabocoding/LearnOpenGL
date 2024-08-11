#pragma once

#include <glm/glm.hpp>

#include "GBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Material.h"
#include "PerspectiveCamera.h"
#include "Moongoose/Core.h"
#include "Light.h"
#include "TextureAtlas.h"

namespace Moongoose {
	class Framebuffer;
	class World;

	class Renderer {

	public:

		struct AtlasBox
		{
			glm::uvec2 topLeft;
			glm::uvec2 bottomRight;
		};

		struct MeshRenderCmd
		{
			MeshRenderCmd(const Ref<VertexArray>& vArray, const Ref<Material>& mat): vertexArray(vArray), material(mat) {}
			MeshRenderCmd(const size_t id, const glm::mat4& transform, const Ref<VertexArray>& vArray, const Ref<Material>& mat):
			id(id),
			transform(transform),
			vertexArray(vArray),
			material(mat) {}

			size_t id = -1;
			glm::mat4 transform = glm::mat4(1.0f);

			Ref<VertexArray> vertexArray;
			Ref<Material> material;
		};

		struct BillboardCmd
		{
			size_t id = -1;
			glm::mat4 transform = glm::mat4(1.0f);
			Ref<Texture2D> texture;
			float scale = 0.75f;
			glm::vec3 tintColor = glm::vec3(1.0f, 1.0f, 1.0f);
		};

	public:
		static Ref<Framebuffer> GetRenderBuffer();
		static void RenderWorld(const Ref<PerspectiveCamera>& camera, const Ref<World>& world);
		static void RenderMesh(const Ref<VertexArray>& vertexArray);

		static void PushMeshRenderCommand(const MeshRenderCmd& cmd);
		static void PushBillboardRenderCommand(const BillboardCmd& cmd);

		static void PushDirectionalLight(const DirectionalLight& light);
		static void PushPointLight(const PointLight& light);
		static void PushSpotLight(const SpotLight& light);

		static void BeginScene();
		static void EndScene();

		static unsigned int GetDrawCount() { return prevDrawCount; }
	private:
		static void InitShadowBuffer();
		static void InitGBuffer();
		static void InitRenderBuffer();
		static void InitPostProcessingBuffer();

		static void SetResolution(glm::uvec2 newResolution);

		static void PrepareLights();
		static void PrepareDirectionalLight(const DirectionalLight& light, const Ref<Shader>& shader);

		static void RenderGBuffer(const Ref<PerspectiveCamera>& camera);
		static void RenderPostProcessing(const Ref<PerspectiveCamera>& camera);
		static void RenderShadowMaps();

		static void ExecuteMeshRenderCommand(const Ref<PerspectiveCamera>& camera, const MeshRenderCmd& cmd);
		static void ExecuteBillboardRenderCommand(const Ref<PerspectiveCamera>& camera, const BillboardCmd& cmd);

		static glm::mat4 GetDirectionalLightProjection(const DirectionalLight& light);
		static glm::mat4 GetPointLightProjection(const PointLight& light);
		static glm::mat4 GetSpotLightProjection(const SpotLight& light);

		static glm::mat4 GetDirectionalLightTransform(const DirectionalLight& light);
		static std::vector<glm::mat4> GetPointLightTransform(const PointLight& light);
		static glm::mat4 GetSpotLightTransform(const SpotLight& light);

		static std::vector<AtlasBox> AllocateTextureAtlas(glm::uvec2 const& atlasSize, std::vector<uint16_t> const& textureSizes);

		static void SetPointLightCount(const Ref<Shader>& shader, const int lightCount);
		static void SetSpotLightCount(const Ref<Shader>& shader, const int lightCount);

		static void AddSpotLight(const size_t index, const Ref<Shader>& shader, const SpotLight& spotLight, const glm::mat4& lightTransform);
		static void AddPointLight(const size_t index, const Ref<Shader>& shader, const PointLight& pointLight, const glm::mat4& lightTransform);

	private:
		static constexpr glm::uvec2 SHADOW_BUFFER_RESOLUTION = { 4096, 4096 };
		static constexpr unsigned int CUBE_SHADOW_MAP_RESOLUTION = 512;

		static std::vector<DirectionalLight> m_DirectionalLights;
		static std::vector<PointLight> m_PointLights;
		static std::vector<SpotLight> m_SpotLights;

		static std::vector<MeshRenderCmd> m_MeshRenderCmds;
		static std::vector<BillboardCmd> m_BillboardRenderCmds;

		static Ref<GBuffer> m_GBuffer;
		static Ref<Framebuffer> m_RenderBuffer;
		static Ref<Framebuffer> m_PostProcessingBuffer;
		static Ref<Framebuffer> m_ShadowBuffer;
		static Ref<Framebuffer> m_PointShadowBuffer;
		static glm::uvec2 m_Resolution;

		static TextureAtlas m_TextureAtlas;

		static unsigned int prevDrawCount;
		static unsigned int currentDrawCount;
	};

}
