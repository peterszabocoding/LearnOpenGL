#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"
#include "Shader.h"
#include "Material.h"
#include "PerspectiveCamera.h"
#include "Moongoose/Core.h"
#include "Light.h"

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
		static Ref<Framebuffer> GetRenderBuffer() { return m_RenderBuffer; }
		static void RenderWorld(const Ref<PerspectiveCamera>& camera, const Ref<World>& world);
		static void RenderMesh(const Ref<VertexArray>& vertexArray);

		static void PushMeshRenderCommand(const MeshRenderCmd& cmd);
		static void PushBillboardRenderCommand(const BillboardCmd& cmd);

		static void PushDirectionalLight(const DirectionalLight& directionalLight);
		static void PushPointLight(const PointLight& pointLight);
		static void PushSpotLight(const SpotLight& spotLight);

		static void BeginScene();
		static void EndScene();
	private:
		static void InitShadowBuffer();
		static void SetResolution(glm::uvec2 newResolution);

		static void PrepareLights();
		static void PrepareDirectionalLight(const DirectionalLight& light, const Ref<Shader>& shader);

		static void RenderShadowMaps();

		static void ExecuteMeshRenderCommand(const Ref<PerspectiveCamera>& camera, const MeshRenderCmd& cmd);
		static void ExecuteBillboardRenderCommand(const Ref<PerspectiveCamera>& camera, const BillboardCmd& cmd);

		static glm::mat4 GetDirectionalLightProjection(const DirectionalLight& light);
		static glm::mat4 GetPointLightProjection(const PointLight& light);
		static glm::mat4 GetSpotLightProjection(const SpotLight& light);

		static glm::mat4 GetDirectionalLightTransform(const DirectionalLight& light);
		static glm::mat4 GetPointLightTransform(const PointLight& light);
		static glm::mat4 GetSpotLightTransform(const SpotLight& light);

		static std::vector<AtlasBox> AllocateTextureAtlas(glm::uvec2 const& atlasSize, std::vector<uint16_t> const& textureSizes);

		static void SetPointLightCount(const Ref<Shader>& shader, const int lightCount);
		static void SetSpotLightCount(const Ref<Shader>& shader, const int lightCount);

		static void AddSpotLight(const size_t index, const Ref<Shader>& shader, const SpotLight& spotLight, const glm::mat4& lightTransform);
		static void AddPointLight(const size_t index, const Ref<Shader>& shader, const PointLight& pointLight, const glm::mat4& lightTransform);

	private:
		static constexpr glm::uvec2 SHADOW_BUFFER_RESOLUTION = { 8192, 4096 };

		static std::vector<DirectionalLight> m_DirectionalLights;
		static std::vector<PointLight> m_PointLights;
		static std::vector<SpotLight> m_SpotLights;

		static std::vector<DirectionalLight> m_ShadowCastingDirectionalLights;
		static std::vector<PointLight> m_ShadowCastingPointLights;
		static std::vector<SpotLight> m_ShadowCastingSpotLights;

		static std::vector<MeshRenderCmd> m_MeshRenderCmds;
		static std::vector<BillboardCmd> m_BillboardRenderCmds;

		static Ref<Framebuffer> m_RenderBuffer;
		static Ref<Framebuffer> m_ShadowBuffer;
		static glm::uvec2 m_Resolution;

		static std::vector<AtlasBox> m_ShadowMapAtlas;
	};

}
