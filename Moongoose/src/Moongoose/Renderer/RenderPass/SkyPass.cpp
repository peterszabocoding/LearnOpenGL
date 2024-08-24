#include "mgpch.h"
#include "RenderPass.h"
#include "Moongoose/Renderer/Framebuffer.h"
#include "Moongoose/Renderer/FramebufferManager.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose
{
	void SkyPass::Render(const RenderPassParams& renderPassParams)
	{
		SkyPassData* skyPassData = static_cast<SkyPassData*>(renderPassParams.additionalData);

		if (!initialized) Init();

		m_RaymarchingBuffer->Bind();
		RenderCommand::SetClearColor(m_RaymarchingBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		m_RaymarchingShader->Bind();
		m_RaymarchingShader->SetFloat("u_Time", skyPassData->time);
		m_RaymarchingShader->SetFloat2("resolution", m_RaymarchingBuffer->GetResolution());
		m_RaymarchingShader->BindTexture(0, m_TransmittanceBuffer->GetColorAttachments()[0]);
		m_RaymarchingShader->BindTexture(1, m_MultiScatteringBuffer->GetColorAttachments()[0]);
		RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_RaymarchingShader->Unbind();
		m_RaymarchingBuffer->Unbind();

		m_SkyBuffer->Bind();
		RenderCommand::SetClearColor(m_SkyBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();
		m_SkyShader->Bind();
		m_SkyShader->SetFloat("u_Time", skyPassData->time);
		m_SkyShader->SetFloat2("resolution", renderPassParams.camera->GetResolution());
		m_SkyShader->SetFloat3("CameraForward", renderPassParams.camera->GetForward());
		m_SkyShader->SetFloat("CameraFOV", renderPassParams.camera->GetFovRad());
		m_SkyShader->SetFloat("CameraFar", renderPassParams.camera->GetFar());
		m_SkyShader->BindTexture(0, m_TransmittanceBuffer->GetColorAttachments()[0]);
		m_SkyShader->BindTexture(1, m_RaymarchingBuffer->GetColorAttachments()[0]);
		RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_SkyShader->Unbind();
		m_SkyBuffer->Unbind();


		const Ref<Shader> bgShader = ShaderManager::GetShaderByType(ShaderType::ATMOSPHERE);

		skyPassData->targetBuffer->Bind();
		bgShader->Bind();
		bgShader->BindTexture(0, m_SkyBuffer->GetColorAttachments()[0]);
		bgShader->SetCamera(
			renderPassParams.camera->GetCameraPosition(),
			renderPassParams.camera->GetViewMatrix(),
			renderPassParams.camera->GetProjection());
		bgShader->DisableFeature(GlFeature::DEPTH_TEST);
		RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);
		bgShader->Unbind();
		skyPassData->targetBuffer->Unbind();
	}

	void SkyPass::Init()
	{
		FramebufferSpecs specs;
		specs.width = 256;
		specs.height = 64;
		specs.attachments = {FramebufferTextureFormat::RGBA32F};

		m_TransmittanceBuffer = FramebufferManager::CreateFramebuffer("TransmittanceBuffer");
		m_TransmittanceBuffer->Configure(specs);

		FramebufferSpecs specs2;
		specs2.width = 32;
		specs2.height = 32;
		specs2.attachments = {FramebufferTextureFormat::RGBA32F};

		m_MultiScatteringBuffer = FramebufferManager::CreateFramebuffer("MultiScatteringBuffer");
		m_MultiScatteringBuffer->Configure(specs2);

		FramebufferSpecs specs3;
		specs3.width = 200;
		specs3.height = 200;
		specs3.attachments = {FramebufferTextureFormat::RGBA32F};

		m_RaymarchingBuffer = FramebufferManager::CreateFramebuffer("RaymarchingBuffer");
		m_RaymarchingBuffer->Configure(specs3);

		FramebufferSpecs specs4;
		specs4.width = 200;
		specs4.height = 200;
		specs4.attachments = {FramebufferTextureFormat::RGBA8};

		m_SkyBuffer = FramebufferManager::CreateFramebuffer("SkyBuffer");
		m_SkyBuffer->Configure(specs4);

		m_TransmittanceShader = CreateRef<Shader>(ShaderType::ATMOSPHERE,
		                                          "shader\\atmos_scattering.vs",
		                                          "shader\\sun_transmittance_lut.fs");

		m_MultiScatteringShader = CreateRef<Shader>(ShaderType::ATMOSPHERE,
		                                            "shader\\atmos_scattering.vs",
		                                            "shader\\multi-scattering_lut.fs");

		m_RaymarchingShader = CreateRef<Shader>(ShaderType::ATMOSPHERE,
		                                        "shader\\atmos_scattering.vs",
		                                        "shader\\raymarch_scattering.fs");

		m_SkyShader = CreateRef<Shader>(ShaderType::ATMOSPHERE,
		                                "shader\\atmos_scattering.vs",
		                                "shader\\atmospheric_sky.fs");

		m_TransmittanceBuffer->Bind();
		RenderCommand::SetClearColor(m_TransmittanceBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		m_TransmittanceShader->Bind();
		m_TransmittanceShader->SetFloat2("resolution", m_TransmittanceBuffer->GetResolution());
		RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_TransmittanceShader->Unbind();
		m_TransmittanceBuffer->Unbind();

		m_MultiScatteringBuffer->Bind();
		RenderCommand::SetClearColor(m_MultiScatteringBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		m_MultiScatteringShader->Bind();
		m_MultiScatteringShader->SetFloat2("resolution", m_MultiScatteringBuffer->GetResolution());
		m_MultiScatteringShader->BindTexture(0, m_TransmittanceBuffer->GetColorAttachments()[0]);
		RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);
		m_MultiScatteringShader->Unbind();
		m_MultiScatteringBuffer->Unbind();

		initialized = true;
	}
}
