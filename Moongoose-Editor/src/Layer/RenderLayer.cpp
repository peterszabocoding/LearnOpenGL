#include <imgui/imgui.h>

#include "RenderLayer.h"
#include "Moongoose/Events/Event.h"


using namespace Moongoose;

void RenderLayer::onAttach()
{
	createRenderBuffer();
	createCamera();

	m_BaseShader = AssetManager::LoadShader();
	m_CheckerTexture = AssetManager::LoadTexture2D("Assets/Texture/checker_2k_c.png", TextureFormat::RGB);
	m_ColorCheckerTexture = AssetManager::LoadTexture2D("Assets/Texture/checker_2k_b.png", TextureFormat::RGB);

	Ref<Moongoose::Material> m_CheckerMaterial = CreateRef<Moongoose::Material>();
	Ref<Moongoose::Material> m_ColorCheckerMaterial = CreateRef<Moongoose::Material>();
	m_CheckerMaterial->Albedo = m_CheckerTexture;
	m_ColorCheckerMaterial->Albedo = m_ColorCheckerTexture;

	{
		Entity monkeyEntity = EntityManager::Get().addEntity("Monkey");
		Entity groundEntity = EntityManager::Get().addEntity("Ground");
		EntityMemoryPool::Get().addComponent<TransformComponent>(monkeyEntity);
		
		auto& groundTransform = EntityMemoryPool::Get().addComponent<TransformComponent>(groundEntity);
		groundTransform.m_Position = glm::vec3(0.0f, -5.0f, 0.0f);
		groundTransform.m_Scale = glm::vec3(15.0f, 1.0f, 15.0f);

		MeshComponent& monkeyMeshComponent = EntityMemoryPool::Get().addComponent<MeshComponent>(monkeyEntity);
		monkeyMeshComponent.m_Mesh = AssetManager::LoadMesh("Assets/Mesh/Monkey.obj");
		monkeyMeshComponent.m_Material = m_ColorCheckerMaterial;
		monkeyMeshComponent.m_Shader = m_BaseShader;

		MeshComponent& groundMeshComponent = EntityMemoryPool::Get().addComponent<MeshComponent>(groundEntity);
		groundMeshComponent.m_Mesh = AssetManager::LoadMesh("Assets/Mesh/Plane.obj");
		groundMeshComponent.m_Material = m_CheckerMaterial;
		groundMeshComponent.m_Shader = m_BaseShader;
	}
}

void RenderLayer::onDetach(){}

void RenderLayer::onUpdate(float deltaTime)
{
	m_EditorCamera->onUpdate(deltaTime);

	glm::mat4 viewMatrix = m_EditorCamera->getViewMatrix();
	glm::vec3 cameraPosition = m_EditorCamera->getCameraPosition();
	glm::mat4 projection = m_EditorCamera->getProjection();

	m_RenderBuffer->Bind();

	RenderCommand::SetClearColor(glm::vec4 { 0.0f, 0.0f, 0.0f, 1.0f });
	RenderCommand::Clear();

	RenderSystem::Run(m_EditorCamera);

	m_RenderBuffer->Unbind();
}

void RenderLayer::onEvent(Moongoose::Event& event)
{
	m_EditorCamera->onEvent(event);
}

void RenderLayer::onImGuiRender()
{
	ImGui::Begin("Render");
	
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
		FramebufferTextureFormat::RGBA8,
		FramebufferTextureFormat::DEPTH24STENCIL8
	};
	specs.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_RenderBuffer = CreateScope<Framebuffer>(specs);
}

void RenderLayer::createCamera()
{
	PerspectiveCamera::Params params;
	params.renderWidth = m_WindowSize.x;
	params.renderHeight = m_WindowSize.y;
	params.startPosition = { 0.0f, 0.0f, 5.0f };
	m_EditorCamera = CreateRef<PerspectiveCamera>(params);
}
