#pragma once

#include "LearnOpenGL/Renderer/Light.h"
#include "LearnOpenGL/Renderer/PerspectiveCamera.h"
#include "LearnOpenGL/Renderer/RenderCommand.h"

namespace LearnOpenGL
{
	class GBuffer;
	class Framebuffer;
	class World;

	struct RenderPassParams
	{
		Ref<World> world;
		Ref<PerspectiveCamera> camera;
		std::vector<MeshCommand> meshCommands;

		void* additionalData;
	};

	class GeometryPass
	{
	public:
		~GeometryPass() = default;
		void Render(Ref<GBuffer> gBuffer, const RenderPassParams& renderPassParams);
	};

	class LightingPass
	{
	public:
		struct LightingPassData
		{
			Ref<Framebuffer> shadowMapBuffer;
			Ref<Framebuffer> pointShadowMapBuffer;

			std::vector<DirectionalLight>& directionalLights;
			std::vector<SpotLight>& spotLights;
			std::vector<PointLight>& pointLights;
		};

	public:
		~LightingPass() = default;
		void Render(Ref<Framebuffer> targetBuffer, RenderPassParams& renderPassParams);

	private:
		static void AddDirectionalLight(
			const DirectionalLight& light,
			const Ref<Shader>& shader,
			const Ref<Framebuffer>& shadowMapBuffer);
		static void AddSpotLight(size_t index, const Ref<Shader>& shader, const SpotLight& spotLight);
		static void AddPointLight(size_t index, const Ref<Shader>& shader, const PointLight& pointLight);
	};

	class PostProcessingSSRPass
	{
	public:
		struct PostProcessingSSRParams
		{
			float intensity = 1.0f;
			float blur = 1.0f;
			float maxDistance = 6.0f;
			float resolution = 0.55f;
			float thickness = 0.085f;
			uint32_t steps = 15;
		};

		struct PostProcessingSSRPassData
		{
			Ref<GBuffer> gBuffer;
			uint32_t renderTexture;
			PostProcessingSSRParams ssrParams;
		};

	public:
		~PostProcessingSSRPass() = default;
		void Render(Ref<Framebuffer> targetBuffer, RenderPassParams& renderPassParams);
	};

	class ShadowMapPass
	{
	public:
		struct ShadowMapPassData
		{
			std::vector<DirectionalLight>& directionalLights;
			std::vector<SpotLight>& spotLights;
			std::vector<PointLight>& pointLights;
		};

	public:
		~ShadowMapPass() = default;
		void Render(Ref<Framebuffer> targetBuffer, RenderPassParams& renderPassParams);

		Ref<Framebuffer> GetShadowBuffer() const { return m_ShadowBuffer; }
		Ref<Framebuffer> GetPointShadowBuffer() const { return m_PointShadowBuffer; }

	private:
		void InitFramebuffer();

	private:
		static constexpr glm::uvec2 SHADOW_BUFFER_RESOLUTION = {4096, 4096};
		static constexpr unsigned int CUBE_SHADOW_MAP_RESOLUTION = 512;

		TextureAtlas m_TextureAtlas;
		Ref<Framebuffer> m_ShadowBuffer;
		Ref<Framebuffer> m_PointShadowBuffer;
	};

	class BillboardPass
	{
	public:
		struct BillboardPassData
		{
			std::vector<BillboardCommand> billboardCommands;
		};

	public:
		~BillboardPass() = default;
		void Render(Ref<Framebuffer> targetBuffer, RenderPassParams& renderPassParams);
	};

	class BoxBlurPass
	{
	public:
		~BoxBlurPass() = default;
		void Render(Ref<Framebuffer> targetBuffer, uint32_t source, char amount, float blurIntensity);
	};

	class PostProcessCombinePass
	{
	public:
		~PostProcessCombinePass() = default;
		void Render(const Ref<Framebuffer> targetBuffer, uint32_t baseTexture, uint32_t textureToBlend,
		            float multiply);
	};

	class SkyPass
	{
	public:
		struct SkyPassData
		{
			float time;
		};

	public:
		~SkyPass() = default;
		void Render(Ref<Framebuffer> targetBuffer, RenderPassParams& renderPassParams);

	private:
		void Init();

	private:
		bool initialized;

		Ref<Framebuffer> m_TransmittanceBuffer;
		Ref<Framebuffer> m_MultiScatteringBuffer;
		Ref<Framebuffer> m_RaymarchingBuffer;
		Ref<Framebuffer> m_SkyBuffer;

		Ref<Shader> m_TransmittanceShader;
		Ref<Shader> m_MultiScatteringShader;
		Ref<Shader> m_RaymarchingShader;
		Ref<Shader> m_SkyShader;
	};
}
