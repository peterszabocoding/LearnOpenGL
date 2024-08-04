#pragma once

#include "Moongoose/Core.h"
#include "VertexArray.h"
#include <glm/glm.hpp>

#include "Material.h"
#include "PerspectiveCamera.h"
#include "Shader.h"

namespace Moongoose {
	enum class LightType : uint8_t;

	enum class ShadowMapResolution : uint16_t
	{
		ULTRA_LOW = 128,
		LOW = 256,
		MEDIUM = 512,
		HIGH = 1024,
		VERY_HIGH = 2048,
		ULTRA_HIGH = 4096
	};

	class Framebuffer;
	class World;

	class Renderer {

	public:

		struct AtlasBox
		{
			glm::uvec2 topLeft;
			glm::uvec2 bottomRight;
		};

		struct Light
		{
			glm::vec3 color = glm::vec3(1.0f);
			float intensity = 1.0f;
			bool isShadowCasting = false;
			ShadowMapResolution shadowMapResolution = ShadowMapResolution::MEDIUM;
		};

		struct DirectionalLight: Light
		{
			glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
			glm::vec3 ambientColor = glm::vec3(1.0f);
			float ambientIntensity = 0.1f;
			
		};

		struct PointLight: Light
		{
			glm::vec3 position = glm::vec3(0.0f);
			float attenuationRadius = 10.0f;
		};

		struct SpotLight: PointLight
		{
			glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
			float attenuationAngle = 0.75f;
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
		static void PreparePointLight(const PointLight& light, const Ref<Shader>& shader);
		static void PrepareSpotLight(const SpotLight& light, const Ref<Shader>& shader);

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

	private:
		static constexpr glm::uvec2 SHADOW_BUFFER_RESOLUTION = { 8192, 8192 };

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
