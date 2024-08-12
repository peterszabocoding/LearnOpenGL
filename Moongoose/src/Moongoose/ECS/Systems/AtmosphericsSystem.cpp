#include "mgpch.h"
#include "AtmosphericsSystem.h"
#include "Moongoose/Renderer/FramebufferManager.h"
#include "Moongoose/Renderer/RenderCommand.h"
#include "Moongoose/Renderer/Renderer.h"
#include "Moongoose/Renderer/ShaderManager.h"
#include "Moongoose/Renderer/MeshPrimitives.h"

namespace Moongoose {
	AtmosphericsSystem::AtmosphericsSystem()
	{
		FramebufferSpecs specs;
		specs.width = 256;
		specs.height = 64;
		specs.attachments = { FramebufferTextureFormat::RGBA32F };

		m_TransmittanceBuffer = FramebufferManager::CreateFramebuffer("TransmittanceBuffer", specs);

		FramebufferSpecs specs2;
		specs2.width = 32;
		specs2.height = 32;
		specs2.attachments = { FramebufferTextureFormat::RGBA32F };

		m_MultiScatteringBuffer = FramebufferManager::CreateFramebuffer("MultiScatteringBuffer", specs2);

		FramebufferSpecs specs3;
		specs3.width = 200;
		specs3.height = 200;
		specs3.attachments = { FramebufferTextureFormat::RGBA32F };

		m_RaymarchingBuffer = FramebufferManager::CreateFramebuffer("RaymarchingBuffer", specs3);

		FramebufferSpecs specs4;
		specs4.width = 200;
		specs4.height = 200;
		specs4.attachments = { FramebufferTextureFormat::RGBA8 };

		m_SkyBuffer = FramebufferManager::CreateFramebuffer("SkyBuffer", specs4);

		m_TransmittanceShader = CreateRef<Shader>(ShaderType::ATMOSPHERE, "shader\\atmos_scattering.vs", "shader\\sun_transmittance_lut.fs");
		m_MultiScatteringShader = CreateRef<Shader>(ShaderType::ATMOSPHERE, "shader\\atmos_scattering.vs", "shader\\multi-scattering_lut.fs");
		m_RaymarchingShader = CreateRef<Shader>(ShaderType::ATMOSPHERE, "shader\\atmos_scattering.vs", "shader\\raymarch_scattering.fs");
		m_SkyShader = CreateRef<Shader>(ShaderType::ATMOSPHERE, "shader\\atmos_scattering.vs", "shader\\atmospheric_sky.fs");
	
		Init();
	}

	Signature AtmosphericsSystem::GetSystemSignature(World* world)
	{
		Signature signature;
		signature.set(world->GetComponentType<AtmosphericsComponent>());
		return signature;
	}

	void AtmosphericsSystem::Init() const
	{
		m_TransmittanceBuffer->Bind();
		RenderCommand::SetClearColor(m_TransmittanceBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		m_TransmittanceShader->Bind();
		m_TransmittanceShader->SetFloat2("resolution", m_TransmittanceBuffer->GetResolution());
		Renderer::RenderMesh(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_TransmittanceShader->Unbind();
		m_TransmittanceBuffer->Unbind();

		m_MultiScatteringBuffer->Bind();
		RenderCommand::SetClearColor(m_MultiScatteringBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		m_MultiScatteringShader->Bind();
		m_MultiScatteringShader->SetFloat2("resolution", m_MultiScatteringBuffer->GetResolution());
		m_MultiScatteringShader->BindTexture(0, m_TransmittanceBuffer->GetColorAttachments()[0]);
		Renderer::RenderMesh(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_MultiScatteringShader->Unbind();
		m_MultiScatteringBuffer->Unbind();
	}

	void AtmosphericsSystem::Update(const Ref<PerspectiveCamera>& camera, glm::vec2 resolution) const
	{
		m_RaymarchingBuffer->Bind();
		RenderCommand::SetClearColor(m_RaymarchingBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		m_RaymarchingShader->Bind();
		m_RaymarchingShader->SetFloat("u_Time", 10.0f);
		m_RaymarchingShader->SetFloat2("resolution", m_RaymarchingBuffer->GetResolution());
		m_RaymarchingShader->BindTexture(0, m_TransmittanceBuffer->GetColorAttachments()[0]);
		m_RaymarchingShader->BindTexture(1, m_MultiScatteringBuffer->GetColorAttachments()[0]);
		Renderer::RenderMesh(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_RaymarchingShader->Unbind();
		m_RaymarchingBuffer->Unbind();

		m_SkyBuffer->Bind();
		RenderCommand::SetClearColor(m_SkyBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();
		m_SkyShader->Bind();
		m_SkyShader->SetFloat("u_Time", 10.0f);
		m_SkyShader->SetFloat2("resolution", resolution);
		m_SkyShader->SetFloat3("CameraForward", camera->GetForward());
		m_SkyShader->SetFloat("CameraFOV", camera->GetFovRad());
		m_SkyShader->SetFloat("CameraFar", camera->GetFar());
		m_SkyShader->BindTexture(0, m_TransmittanceBuffer->GetColorAttachments()[0]);
		m_SkyShader->BindTexture(1, m_RaymarchingBuffer->GetColorAttachments()[0]);
		Renderer::RenderMesh(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_SkyShader->Unbind();
		m_SkyBuffer->Unbind();
	}

	void AtmosphericsSystem::Run(const Ref<PerspectiveCamera>& camera) const
	{
		if (m_Entities.empty()) return;

		const Ref<Shader> bgShader = ShaderManager::GetShaderByType(ShaderType::ATMOSPHERE);

		bgShader->Bind();
		bgShader->BindTexture(0, m_SkyBuffer->GetColorAttachments()[0]);
		bgShader->SetCamera(camera->GetCameraPosition(), camera->GetViewMatrix(), camera->GetProjection());
		bgShader->DisableFeature(GlFeature::DEPTH_TEST);
		Renderer::RenderMesh(QuadMesh().GetSubmeshes()[0]->vertexArray);
		bgShader->Unbind();
	}

}
