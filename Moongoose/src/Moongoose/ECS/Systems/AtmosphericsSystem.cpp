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
		specs.Width = 256;
		specs.Height = 64;
		specs.Attachments = { FramebufferTextureFormat::RGBA32F };

		m_TransmittanceBuffer = FramebufferManager::CreateFramebuffer("TransmittanceBuffer", specs);

		FramebufferSpecs specs2;
		specs2.Width = 32;
		specs2.Height = 32;
		specs2.Attachments = { FramebufferTextureFormat::RGBA32F };

		m_MultiScatteringBuffer = FramebufferManager::CreateFramebuffer("MultiScatteringBuffer", specs2);


		FramebufferSpecs specs3;
		specs3.Width = 200;
		specs3.Height = 200;
		specs3.Attachments = { FramebufferTextureFormat::RGBA32F };

		m_RaymarchingBuffer = FramebufferManager::CreateFramebuffer("RaymarchingBuffer", specs3);

		FramebufferSpecs specs4;
		specs4.Width = 200;
		specs4.Height = 200;
		specs4.Attachments = { FramebufferTextureFormat::RGBA8 };

		m_SkyBuffer = FramebufferManager::CreateFramebuffer("SkyBuffer", specs4);


		m_TransmittanceShader = CreateRef<Shader>(ShaderType::ATMOSPHERE, "shader\\atmos_scattering.vs", "shader\\sun_transmittance_lut.fs");
		m_MultiScatteringShader = CreateRef<Shader>(ShaderType::ATMOSPHERE, "shader\\atmos_scattering.vs", "shader\\multi-scattering_lut.fs");
		m_RaymarchingShader = CreateRef<Shader>(ShaderType::ATMOSPHERE, "shader\\atmos_scattering.vs", "shader\\raymarch_scattering.fs");
		m_SkyShader = CreateRef<Shader>(ShaderType::ATMOSPHERE, "shader\\atmos_scattering.vs", "shader\\atmospheric_sky.fs");
	
		Init();
	}

	AtmosphericsSystem::~AtmosphericsSystem()
	{
	}

	Signature AtmosphericsSystem::GetSystemSignature(World* world)
	{
		Signature signature;
		signature.set(world->GetComponentType<AtmosphericsComponent>());
		return signature;
	}

	void AtmosphericsSystem::Init()
	{
		//if (isLUTGenerated) return;

		m_TransmittanceBuffer->Bind();
		RenderCommand::SetClearColor(m_TransmittanceBuffer->GetSpecs().ClearColor);
		RenderCommand::Clear();

		m_TransmittanceShader->Bind();
		m_TransmittanceShader->UploadUniformFloat2("resolution", m_TransmittanceBuffer->GetResolution());
		Renderer::RenderMesh(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_TransmittanceShader->Unbind();

		m_TransmittanceBuffer->Unbind();


		m_MultiScatteringBuffer->Bind();
		RenderCommand::SetClearColor(m_MultiScatteringBuffer->GetSpecs().ClearColor);
		RenderCommand::Clear();

		m_MultiScatteringShader->Bind();
		m_MultiScatteringShader->UploadUniformFloat2("resolution", m_MultiScatteringBuffer->GetResolution());
		m_MultiScatteringShader->BindTexture(0, m_TransmittanceBuffer->GetColorAttachments()[0]);
		Renderer::RenderMesh(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_MultiScatteringShader->Unbind();

		m_MultiScatteringBuffer->Unbind();

		isLUTGenerated = true;
	}

	void AtmosphericsSystem::Update(const Ref<PerspectiveCamera>& camera, glm::vec2 resolution)
	{
		m_RaymarchingBuffer->Bind();
		RenderCommand::SetClearColor(m_RaymarchingBuffer->GetSpecs().ClearColor);
		RenderCommand::Clear();

		m_RaymarchingShader->Bind();
		m_RaymarchingShader->UploadUniformFloat("u_Time", 10.0f);
		m_RaymarchingShader->UploadUniformFloat2("resolution", m_RaymarchingBuffer->GetResolution());
		m_RaymarchingShader->BindTexture(0, m_TransmittanceBuffer->GetColorAttachments()[0]);
		m_RaymarchingShader->BindTexture(1, m_MultiScatteringBuffer->GetColorAttachments()[0]);
		Renderer::RenderMesh(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_RaymarchingShader->Unbind();
		m_RaymarchingBuffer->Unbind();

		m_SkyBuffer->Bind();
		RenderCommand::SetClearColor(m_SkyBuffer->GetSpecs().ClearColor);
		RenderCommand::Clear();
		m_SkyShader->Bind();
		m_SkyShader->UploadUniformFloat("u_Time", 10.0f);
		m_SkyShader->UploadUniformFloat2("resolution", resolution);
		m_SkyShader->UploadUniformFloat3("CameraForward", camera->getForward());
		m_SkyShader->UploadUniformFloat("CameraFOV", camera->getFOVRad());
		m_SkyShader->UploadUniformFloat("CameraFar", camera->getFar());
		m_SkyShader->BindTexture(0, m_TransmittanceBuffer->GetColorAttachments()[0]);
		m_SkyShader->BindTexture(1, m_RaymarchingBuffer->GetColorAttachments()[0]);
		Renderer::RenderMesh(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_SkyShader->Unbind();
		m_SkyBuffer->Unbind();
	}

	void AtmosphericsSystem::Run(const Ref<PerspectiveCamera>& camera, Ref<World> world)
	{
		if (m_Entities.size() <= 0) return;

		for (auto const& entity : m_Entities)
		{
			TransformComponent cTransform = world->GetComponent<TransformComponent>(entity);
			AtmosphericsComponent cAtmos = world->GetComponent<AtmosphericsComponent>(entity);
			Ref<Shader> bgShader = ShaderManager::GetShaderByType(ShaderType::ATMOSPHERE);

			bgShader->Bind();
			bgShader->BindTexture(0, m_SkyBuffer->GetColorAttachments()[0]);
			bgShader->SetCamera(camera->getCameraPosition(), camera->getViewMatrix(), camera->getProjection());
			bgShader->SetDepthTest(false);
			Renderer::RenderMesh(QuadMesh().GetSubmeshes()[0]->vertexArray);
			bgShader->Unbind();
		}	
	}

}
