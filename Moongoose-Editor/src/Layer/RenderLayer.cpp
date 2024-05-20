#include <imgui/imgui.h>

#include "ImGuizmo.h"
#include "RenderLayer.h"
#include "Moongoose/Events/Event.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"
#include "Moongoose/ECS/WorldManager.h"

using namespace Moongoose;

void RenderLayer::onAttach()
{
	createRenderBuffer();
	createPreviewRenderBuffer();
	createCamera();

	auto& assetManager = AssetManager::Get();
	auto& shaderManager = ShaderManager::Get();
	auto& worldManager = WorldManager::Get();

	m_World = worldManager.CreateWorld("Main_Scene");

	shaderManager.AssignShaderToType(ShaderType::STATIC, "Shader\\shader.vert", "Shader\\shader.frag");

	m_CheckerTexture = assetManager.LoadAsset<Moongoose::Texture2D>("T_Checker_01_C", "Assets\\Texture\\checker_2k_c.png");
	m_ColorCheckerTexture = assetManager.LoadAsset<Moongoose::Texture2D>("T_Checker_01_B", "Assets\\Texture\\checker_2k_b.png");

	Ref<Moongoose::Material> m_CheckerMaterial = assetManager.LoadAsset<Moongoose::Material>("M_Checker", "Assets\\Material\\");
	Ref<Moongoose::Material> m_ColorCheckerMaterial = assetManager.LoadAsset<Moongoose::Material>("M_Color_Checker", "Assets\\Material\\");
	
	m_CheckerMaterial->setAlbedo(m_CheckerTexture);
	m_ColorCheckerMaterial->setAlbedo(m_ColorCheckerTexture);

	assetManager.SaveAsset<Moongoose::Material>(m_CheckerMaterial);
	assetManager.SaveAsset<Moongoose::Material>(m_ColorCheckerMaterial);

	auto monkeyHat = assetManager.LoadAsset<Mesh>("SM_Monkey_Hat", "Assets\\Mesh\\Monkey_Hat.fbx");
	monkeyHat->GetMaterial(0)->setAlbedo(m_ColorCheckerTexture);
	monkeyHat->GetMaterial(1)->setAlbedo(m_CheckerTexture);
	assetManager.SaveAsset(monkeyHat->GetMaterial(0));
	assetManager.SaveAsset(monkeyHat->GetMaterial(1));

	auto planeMesh = assetManager.LoadAsset<Mesh>("SM_Plane", "Assets\\Mesh\\Plane.obj");
	planeMesh->SetMaterial(0, m_CheckerMaterial);

	auto& loadedAssets = assetManager.GetLoadedAssets();

	Entity monkeyHatEntity = m_World->CreateEntity("Monkey_Hat");
	Entity groundEntity = m_World->CreateEntity("Ground");

	Entity directionalLight = m_World->CreateEntity("Directional Light");
	m_World->AddComponent<LightComponent>(directionalLight);

	auto& dirLightTransform = m_World->GetComponent<TransformComponent>(directionalLight);
	dirLightTransform.m_Rotation += glm::vec3(45.0f, -135.0f, 0.0f);

	auto& dirLightComponent = m_World->GetComponent<LightComponent>(directionalLight);
	dirLightComponent.m_Type = LightType::DIRECTIONAL;

	auto& groundTransform = m_World->GetComponent<TransformComponent>(groundEntity);
	groundTransform.m_Position = glm::vec3(0.0f, -5.0f, 0.0f);
	groundTransform.m_Scale = glm::vec3(15.0f, 1.0f, 15.0f);
	
	MeshComponent& monkeyHatMeshComponent = m_World->AddComponent<MeshComponent>(monkeyHatEntity);
	monkeyHatMeshComponent.m_Mesh = monkeyHat;

	MeshComponent& groundMeshComponent = m_World->AddComponent<MeshComponent>(groundEntity);
	groundMeshComponent.m_Mesh = planeMesh;
}

void RenderLayer::onDetach(){}

void RenderLayer::onUpdate(float deltaTime)
{
	m_EditorCamera->setCameraActive(isMouseInWindow());
	m_EditorCamera->onUpdate(deltaTime);
	m_RenderBuffer->Bind();

	RenderCommand::SetClearColor(glm::vec4 { 0.0f, 0.0f, 0.0f, 1.0f });
	RenderCommand::Clear();
	m_RenderBuffer->ClearAttachment(1, -1);

	RenderSystem::Run(m_EditorCamera, m_World);

	auto [mx, my] = ImGui::GetMousePos();
	mx -= m_ViewportBounds[0].x;
	my -= m_ViewportBounds[0].y;
	glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	my = viewportSize.y - my;

	m_WindowMousePos = { (int)mx, (int)my };
	if (isMouseInWindow())
	{
		int pixelData = m_RenderBuffer->ReadPixel(1, m_WindowMousePos.x, m_WindowMousePos.y);
		m_HoveredEntityId = pixelData;
		//EntityManager::Get().setSelectedEntity(pixelData);
		//m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
	}

	m_RenderBuffer->Unbind();
}

void RenderLayer::onEvent(Moongoose::Event& event)
{
	m_EditorCamera->onEvent(event);
	Moongoose::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Moongoose::MousePressedEvent>(BIND_EVENT_FUNC(RenderLayer::onMouseButtonPresed));
	dispatcher.Dispatch<Moongoose::KeyPressedEvent>(BIND_EVENT_FUNC(RenderLayer::onKeyPressed));
}

void RenderLayer::onImGuiRender()
{
	ImGuizmo::BeginFrame();
	ImGui::Begin("Render");
	
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	if (windowSize.x != m_WindowSize.x || windowSize.y != m_WindowSize.y)
	{
		m_WindowSize = { windowSize.x, windowSize.y };
		m_EditorCamera->setRenderResolution(m_WindowSize.x, m_WindowSize.y);
		createRenderBuffer();
	}

	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();

	m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
	
	ImGui::Image((void*) m_RenderBuffer->GetColorAttachments()[0],
		windowSize,
		ImVec2(0, 1),
		ImVec2(1, 0));


	// Gizmo
	size_t selectedEntity = m_World->GetSelectedEntity();
	if (selectedEntity != -1)
	{
		auto& entityTransform = m_World->GetComponent<Moongoose::TransformComponent>(selectedEntity);

		ImGuizmo::AllowAxisFlip(false);
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(
			m_ViewportBounds[0].x, 
			m_ViewportBounds[0].y, 
			m_ViewportBounds[1].x - m_ViewportBounds[0].x, 
			m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		auto& transform = entityTransform.getTransform();

		ImGuizmo::Manipulate(
			glm::value_ptr(m_EditorCamera->getViewMatrix()),
			glm::value_ptr(m_EditorCamera->getProjection()),
			ImGuizmo::OPERATION(m_GizmoMode),
			ImGuizmo::LOCAL, 
			glm::value_ptr(transform),
		nullptr, 
			nullptr);

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, rotation, scale;
			TransformComponent::DecomposeTransform(transform,translation,rotation,scale);
			glm::vec3 deltaRotation = glm::degrees(rotation) - entityTransform.m_Rotation;

			entityTransform.m_Position = translation;
			entityTransform.m_Rotation += deltaRotation;
			entityTransform.m_Scale = scale;
		}
	}

	ImGui::End();

	ImGui::Begin("Render Debug");
	ImGui::Text("Hovered entity: %d", m_HoveredEntityId);
	ImGui::Text("Mouse: X: %f Y: %f", m_WindowMousePos.x, m_WindowMousePos.y);
	ImGui::Text("Is mouse inside window: %s", isMouseInWindow() ? "True" : "False");
	ImGui::End();

	ImGui::Begin("Preview");
	ImGui::Text("Preview");
	ImGui::End();
}

void RenderLayer::createRenderBuffer()
{
	Moongoose::FramebufferSpecs specs;
	specs.Width = m_WindowSize.x;
	specs.Height = m_WindowSize.y;
	specs.Attachments = {
		FramebufferTextureFormat::RGBA8,
		FramebufferTextureFormat::RED_INTEGER,
		FramebufferTextureFormat::DEPTH24STENCIL8
	};
	specs.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_RenderBuffer = CreateScope<Framebuffer>(specs);
}

void RenderLayer::createPreviewRenderBuffer()
{
	Moongoose::FramebufferSpecs specs;
	specs.Width = m_WindowSize.x;
	specs.Height = m_WindowSize.y;
	specs.Attachments = {
		FramebufferTextureFormat::RGBA8,
		FramebufferTextureFormat::RED_INTEGER,
		FramebufferTextureFormat::DEPTH24STENCIL8
	};
	specs.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_PreviewRenderBuffer = CreateScope<Framebuffer>(specs);
}

void RenderLayer::createCamera()
{
	PerspectiveCamera::Params params;
	params.renderWidth = m_WindowSize.x;
	params.renderHeight = m_WindowSize.y;
	params.startPosition = { 0.0f, 0.0f, 5.0f };
	m_EditorCamera = CreateRef<PerspectiveCamera>(params);
}

bool RenderLayer::onKeyPressed(Moongoose::KeyPressedEvent& event)
{
	if (Input::IsMousePressed(MG_MOUSE_BUTTON_RIGHT)) return false;

	if (event.getKeyCode() == MG_KEY_W)
	{
		m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;
		return false;
	}

	if (event.getKeyCode() == MG_KEY_E)
	{
		m_GizmoMode = ImGuizmo::OPERATION::SCALE;
		return false;
	}

	if (event.getKeyCode() == MG_KEY_R)
	{
		m_GizmoMode = ImGuizmo::OPERATION::ROTATE;
		return false;
	}

	if (event.getKeyCode() == MG_KEY_ESCAPE)
	{
		m_World->SetSelectedEntity(-1);
		return false;
	}
}

bool RenderLayer::onMouseButtonPresed(Moongoose::MousePressedEvent& event)
{
	if (m_HoveredEntityId != -1 && Input::IsMousePressed(MG_MOUSE_BUTTON_LEFT) && Input::IsKeyPressed(MG_KEY_LEFT_SHIFT))
	{
		m_World->SetSelectedEntity(m_HoveredEntityId);
	}
	return false;
}

bool RenderLayer::isMouseInWindow() const
{
	glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	return m_WindowMousePos.x >= 0 && m_WindowMousePos.y >= 0 && m_WindowMousePos.x < (int)viewportSize.x && m_WindowMousePos.y < (int)viewportSize.y;
}
