#include "RenderLayer.h"
#include <imgui/imgui.h>

#include "Moongoose/Events/Event.h"

void RenderLayer::onAttach()
{
	createRenderBuffer();
	createCamera();

	m_BaseShader = new Moongoose::Shader(
		Moongoose::ShaderSpecs{ 
			Moongoose::ShaderType::STATIC, 
			"Shader/shader.vert", 
			"Shader/shader.frag" 
		});

	m_Mesh = Moongoose::AssetManager::LoadMesh("Assets/Monkey.obj");
}

void RenderLayer::onDetach(){}

void RenderLayer::onUpdate(float deltaTime)
{
	m_EditorCamera->onUpdate(deltaTime);

	glm::mat4 viewMatrix = m_EditorCamera->getViewMatrix();
	glm::vec3 cameraPosition = m_EditorCamera->getCameraPosition();
	glm::mat4 projection = m_EditorCamera->getProjection();

	Moongoose::Transform transform;
	transform.position += glm::vec3(0.0f, 0.0f, -5.0f);

	m_RenderBuffer->Bind();

	Moongoose::RenderCommand::SetClearColor(glm::vec4 { 0.0f, 0.0f, 0.0f, 1.0f });
	Moongoose::RenderCommand::Clear();

	m_BaseShader->Bind();
	m_BaseShader->SetCamera(cameraPosition, viewMatrix, projection);
	m_BaseShader->SetModelTransform(transform.getModelMatrix());

	Moongoose::Renderer::RenderMesh(m_Mesh->GetVertexArray());

	m_BaseShader->Unbind();
	m_RenderBuffer->Unbind();
}

void RenderLayer::onEvent(Moongoose::Event& event)
{
	m_EditorCamera->onEvent(event);
}

void RenderLayer::onImGuiRender()
{
	ImGui::Begin("RenderLayer");
	
	ImVec2 windowSize = ImGui::GetWindowSize();
	windowSize.x -= 35.0f;
	windowSize.y -= 35.0f;

	if (windowSize.x != m_WindowSize.x || windowSize.y != m_WindowSize.y)
	{
		m_WindowSize = { windowSize.x, windowSize.y };
		m_EditorCamera->setRenderResolution(m_WindowSize.x, m_WindowSize.y);
		createRenderBuffer();
	}
	
	ImGui::Image((void*) m_RenderBuffer->GetColorAttachments()[0],
		windowSize,
		ImVec2(0, 1),
		ImVec2(1, 0));

	ImGui::End();
}

void RenderLayer::createRenderBuffer()
{
	Moongoose::FramebufferSpecs specs;
	specs.Width = m_WindowSize.x;
	specs.Height = m_WindowSize.y;
	specs.Attachments = {
		Moongoose::FramebufferTextureFormat::RGBA8,
		Moongoose::FramebufferTextureFormat::DEPTH24STENCIL8
	};
	specs.ClearColor = { 1.0f, 0.0f, 1.0f, 1.0f };
	m_RenderBuffer = CreateScope<Moongoose::Framebuffer>(specs);
}

void RenderLayer::createCamera()
{
	Moongoose::PerspectiveCamera::Params params;
	params.renderWidth = m_WindowSize.x;
	params.renderHeight = m_WindowSize.y;
	params.startPosition = { 0.0f, 0.0f, -1.0f };
	m_EditorCamera = CreateScope<Moongoose::PerspectiveCamera>(params);
}
