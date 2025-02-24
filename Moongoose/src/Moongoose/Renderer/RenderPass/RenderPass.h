#pragma once

#include "Moongoose/Renderer/Light.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"
#include "Moongoose/Renderer/RenderCommand.h"

namespace Moongoose
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

	class RenderPass
	{
	public:
		virtual ~RenderPass()
		{
		};
		virtual void Render(const RenderPassParams& renderPassParams) = 0;

		virtual void Resize(const glm::uvec2& resolution)
		{
		}
	};

	class GeometryPass : public RenderPass
	{
	public:
		~GeometryPass() override = default;
		virtual void Render(const RenderPassParams& renderPassParams) override;
		virtual void Resize(const glm::uvec2& resolution) override;

		[[nodiscard]] Ref<GBuffer> GetGBuffer() const { return m_GBuffer; }

	private:
		Ref<GBuffer> m_GBuffer;
	};

	class LightingPass : public RenderPass
	{
	public:
		struct LightingPassData
		{
			Ref<Framebuffer> shadowMapBuffer;
			Ref<Framebuffer> pointShadowMapBuffer;

			std::vector<DirectionalLight>& directionalLights;
			std::vector<SpotLight>& spotLights;
			std::vector<PointLight>& pointLights;

			Ref<Framebuffer> targetBuffer;
		};

	public:
		~LightingPass() override = default;

		virtual void Render(const RenderPassParams& renderPassParams) override;

	private:
		static void AddDirectionalLight(
			const DirectionalLight& light,
			const Ref<Shader>& shader,
			const Ref<Framebuffer>& shadowMapBuffer);
		static void AddSpotLight(size_t index, const Ref<Shader>& shader, const SpotLight& spotLight);
		static void AddPointLight(size_t index, const Ref<Shader>& shader, const PointLight& pointLight);
	};

	class SsrPass : public RenderPass
	{
	public:
		struct SsrParams
		{
			float maxDistance = 6.0f;
			float resolution = 0.55f;
			uint32_t steps = 15;
			float thickness = 0.085f;
		};

		struct SsrPassData
		{
			Ref<GBuffer> gBuffer;
			uint32_t renderTexture;
			SsrParams ssrParams;
		};

	public:
		~SsrPass() override
		{
		}

		virtual void Render(const RenderPassParams& renderPassParams) override;
		virtual void Resize(const glm::uvec2& resolution) override;

		Ref<Framebuffer> GetFramebuffer() const { return framebuffer; }

	private:
		void InitFramebuffer(glm::uvec2 resolution);

	private:
		Ref<Framebuffer> framebuffer;
	};

	class ShadowMapPass : public RenderPass
	{
	public:
		struct ShadowMapPassData
		{
			std::vector<DirectionalLight>& directionalLights;
			std::vector<SpotLight>& spotLights;
			std::vector<PointLight>& pointLights;
		};

	public:
		~ShadowMapPass() override
		{
		};
		virtual void Render(const RenderPassParams& renderPassParams) override;

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

	class BillboardPass : public RenderPass
	{
	public:
		struct BillboardPassData
		{
			Ref<Framebuffer> targetBuffer;
			std::vector<BillboardCommand> billboardCommands;
		};

	public:
		~BillboardPass() override = default;
		virtual void Render(const RenderPassParams& renderPassParams) override;
	};

	class BoxBlurPass : public RenderPass
	{
	public:
		struct BoxBlurPassData
		{
			Ref<Framebuffer> targetBuffer;
			uint32_t colorTexture;
		};

	public:
		~BoxBlurPass() override = default;
		virtual void Render(const RenderPassParams& renderPassParams) override;
	};

	class SkyPass : public RenderPass
	{
	public:
		struct SkyPassData
		{
			Ref<Framebuffer> targetBuffer;
			float time;
		};

	public:
		~SkyPass() override = default;
		virtual void Render(const RenderPassParams& renderPassParams) override;

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
