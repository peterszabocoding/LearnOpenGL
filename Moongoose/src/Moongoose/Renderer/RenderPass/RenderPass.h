#pragma once

#include "Moongoose/Renderer/Framebuffer.h"
#include "Moongoose/Renderer/GBuffer.h"
#include "Moongoose/Renderer/Light.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"
#include "Moongoose/Renderer/RenderCommand.h"

namespace Moongoose
{
	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;
		virtual void Render(void* renderSpecs) = 0;
	};

	class GeometryPass : public RenderPass
	{
	public:
		struct Specs
		{
			glm::uvec2 resolution;
			const Ref<PerspectiveCamera> camera;
			const std::vector<MeshCommand>& meshCommands;
		};

	public:
		~GeometryPass() override = default;
		virtual void Render(void* renderSpecs) override;

		[[nodiscard]] Ref<GBuffer> GetGBuffer() const { return m_GBuffer; }

	private:
		Ref<GBuffer> m_GBuffer;
	};

	class SSRPass : public RenderPass
	{
	public:
		struct Params
		{
			glm::uvec2 resolution;
			const Ref<PerspectiveCamera> camera;
			Ref<GBuffer> gBuffer;
			uint32_t renderTexture;
		};

	public:
		~SSRPass() override = default;
		virtual void Render(void* renderParams) override;

		Ref<Framebuffer> GetFramebuffer() const { return framebuffer; }

	private:
		void InitFramebuffer(glm::uvec2 resolution);

	private:
		Ref<Framebuffer> framebuffer;
	};

	class ShadowMapPass : public RenderPass
	{
	public:
		struct Params
		{
			glm::uvec2 resolution;
			const Ref<PerspectiveCamera> camera;
			const std::vector<MeshCommand>& meshCommands;

			std::vector<DirectionalLight>& directionalLights;
			std::vector<SpotLight>& spotLights;
			std::vector<PointLight>& pointLights;
		};

	public:
		~ShadowMapPass() override = default;
		virtual void Render(void* renderParams) override;

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
}
