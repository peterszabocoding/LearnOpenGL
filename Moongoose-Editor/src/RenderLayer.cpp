#include "RenderLayer.h"
#include <imgui/imgui.h>

RenderLayer::RenderLayer() : Layer("RenderLayer")
{
}

RenderLayer::~RenderLayer()
{
}

void RenderLayer::onAttach()
{
	Moongoose::FramebufferSpecs specs{};
	specs.Width = 1280;
	specs.Height = 720;
	specs.Attachments = { 
		Moongoose::FramebufferTextureFormat::RGBA8, 
		Moongoose::FramebufferTextureFormat::DEPTH24STENCIL8 
	};
	specs.ClearColor = { 1.0f, 0.0f, 1.0f, 1.0f };
	m_RenderBuffer = new Moongoose::Framebuffer(specs);


	Moongoose::PerspectiveCamera::Params params{};
	params.renderWidth = specs.Width;
	params.renderHeight = specs.Height;
	params.startPosition = { 0.0f, 1.0f, 0.0f };
	m_EditorCamera = new Moongoose::PerspectiveCamera(params);

	m_BaseShader = new Moongoose::Shader(
		Moongoose::ShaderSpecs{ 
			Moongoose::ShaderType::STATIC, 
			"Shader/shader.vert", 
			"Shader/shader.frag" 
		});

	m_Mesh = Moongoose::Triangle();

}

void RenderLayer::onDetach()
{
	delete m_RenderBuffer;
	delete m_EditorCamera;
}

void RenderLayer::onUpdate(float deltaTime)
{
	m_EditorCamera->onUpdate(deltaTime);

	glm::mat4 viewMatrix = m_EditorCamera->calculateViewMatrix();
	glm::vec3 cameraPosition = m_EditorCamera->getCameraPosition();
	glm::mat4 projection = m_EditorCamera->getProjection();

	m_RenderBuffer->Bind();

	Moongoose::RenderCommand::SetClearColor(glm::vec4 { 0.0f, 0.0f, 0.0f, 1.0f });
	Moongoose::RenderCommand::Clear();

	m_BaseShader->SetCamera(cameraPosition, viewMatrix, projection);

	Moongoose::Renderer::RenderMesh(m_Mesh.GetVertexArray());

	m_RenderBuffer->Unbind();
}

void RenderLayer::onEvent(Moongoose::Event& event)
{
}

void RenderLayer::onImGuiRender()
{
	ImGui::Begin("RenderLayer");
	ImVec2 imageSize = ImVec2(1280, 720);
	ImGui::Image((void*)m_RenderBuffer->GetColorAttachments()[0], imageSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}
