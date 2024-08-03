#pragma once

#include "Moongoose/Core.h"
#include "VertexArray.h"
#include <glm/glm.hpp>

#include "Material.h"
#include "PerspectiveCamera.h"
#include "Shader.h"

namespace Moongoose {

	class Framebuffer;
	class World;

	class Renderer {

	public:

		struct Light
		{
			glm::vec3 color = glm::vec3(1.0f);
			float intensity = 1.0f;
			bool isShadowCasting = false;
		};

		struct DirectionalLight: Light
		{
			glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
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
		static void SetResolution(glm::uvec2 newResolution);

		static void PrepareLights();

		static void ExecuteMeshRenderCommand(const Ref<PerspectiveCamera>& camera, const MeshRenderCmd& cmd);
		static void ExecuteBillboardRenderCommand(const Ref<PerspectiveCamera>& camera, const BillboardCmd& cmd);

	private:
		static std::vector<DirectionalLight> m_DirectionalLights;
		static std::vector<PointLight> m_PointLights;
		static std::vector<SpotLight> m_SpotLights;

		static std::vector<MeshRenderCmd> m_MeshRenderCmds;
		static std::vector<BillboardCmd> m_BillboardRenderCmds;

		static Ref<Framebuffer> m_RenderBuffer;
		static glm::uvec2 m_Resolution;
	};

}
