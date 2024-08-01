#include <imgui/imgui.h>

#include "ImGuizmo.h"
#include "RenderLayer.h"
#include "Moongoose/Events/Event.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"
#include "Moongoose/ECS/WorldManager.h"
#include "Moongoose/Util/FileSystem.h"
#include "GUI/GuiWidgets.h"
#include "Platform/PlatformUtils.h"

using namespace Moongoose;

void RenderLayer::onAttach()
{
	createRenderBuffer();
	createPreviewRenderBuffer();
	createCamera();

	AssetManager::Get().BuildAssetRegistry();
	ShaderManager::AssignShaderToType(ShaderType::STATIC, "shader\\shader.vert", "shader\\shader.frag");
	ShaderManager::AssignShaderToType(ShaderType::BILLBOARD, "shader\\billboard.vs", "shader\\billboard.fs");
	ShaderManager::AssignShaderToType(ShaderType::ATMOSPHERE, "shader\\atmos_scattering.vs", "shader\\atmos_scattering.frag");
}

void RenderLayer::onUpdate(float deltaTime)
{
	m_Time += deltaTime;
	if (m_Time >= 50.0) m_Time = 0.0;

	if (!WorldManager::Get().isWorldOpened()) return;

	m_AtmosphericsSystem->Update(m_EditorCamera, m_RenderBuffer->GetResolution());

	m_EditorCamera->setCameraActive(isMouseInWindow());
	m_EditorCamera->onUpdate(deltaTime);
	
	m_RenderBuffer->Bind();

	RenderCommand::SetClearColor(glm::vec4 { 0.0f, 0.0f, 0.0f, 1.0f });
	RenderCommand::Clear();
	m_RenderBuffer->ClearAttachment(1, -1);

	m_AtmosphericsSystem->Run(m_EditorCamera, WorldManager::Get().GetLoadedWorld());
	m_LightSystem->Run(m_EditorCamera, WorldManager::Get().GetLoadedWorld());
	m_RenderSystem->Run(m_EditorCamera, WorldManager::Get().GetLoadedWorld());
	m_BillboardSystem->Run(m_EditorCamera, WorldManager::Get().GetLoadedWorld());

	auto [mx, my] = ImGui::GetMousePos();
	mx -= m_ViewportBounds[0].x;
	my -= m_ViewportBounds[0].y;
	glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	my = viewportSize.y - my;

	m_WindowMousePos = { (int)mx, (int)my };
	if (isMouseInWindow())
	{
		m_HoveredEntityId = m_RenderBuffer->ReadPixel(1, m_WindowMousePos.x, m_WindowMousePos.y);
	}

	m_RenderBuffer->Unbind();
}

void RenderLayer::onEvent(Event& event)
{
	m_EditorCamera->onEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MousePressedEvent>(BIND_EVENT_FUNC(RenderLayer::onMouseButtonPresed));
	dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FUNC(RenderLayer::onKeyPressed));
}

void RenderLayer::onImGuiRender()
{
	renderToolbarMenu();

	ImGuizmo::BeginFrame();
	ImGui::Begin("Render");

	if (!WorldManager::IsWorldOpened())
	{
		ImGui::End();
		return;
	}

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

	renderGizmo();
	renderDebugInfo(viewportMinRegion.x, viewportMinRegion.y);

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

	m_RenderBuffer = FramebufferManager::CreateFramebuffer("RenderBuffer", specs);
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
	m_PreviewRenderBuffer = FramebufferManager::CreateFramebuffer("PreviewBuffer", specs);
}

void RenderLayer::createCamera()
{
	PerspectiveCamera::Params params;
	params.renderWidth = m_WindowSize.x;
	params.renderHeight = m_WindowSize.y;
	params.startPosition = { 0.0f, 0.0f, 5.0f };
	m_EditorCamera = CreateRef<PerspectiveCamera>(params);
}

void RenderLayer::renderToolbarMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New World"))
			{
				WorldManager::CreateWorld("New World");
			}
			if (ImGui::MenuItem("Open World"))
			{
				std::string worldFilePath = Moongoose::FileDialogs::OpenFile(".mgworld");

				if (!worldFilePath.empty())
				{
					WorldManager::LoadWorld(worldFilePath);
					m_LightSystem = WorldManager::GetLoadedWorld()->GetSystem<LightSystem>();
					m_RenderSystem = WorldManager::GetLoadedWorld()->GetSystem<RenderSystem>();
					m_BillboardSystem = WorldManager::GetLoadedWorld()->GetSystem<BillboardSystem>();
					m_AtmosphericsSystem = WorldManager::GetLoadedWorld()->GetSystem<AtmosphericsSystem>();
				}
			}

			if (ImGui::MenuItem("Save World"))
			{
				std::string worldName = WorldManager::GetLoadedWorld()->m_Name;
				WorldManager::SaveWorld("Content\\Worlds\\" + worldName + ".mgworld");
			}
			if (ImGui::MenuItem("Save World As..")) {}

			ImGui::Separator();
			if (ImGui::MenuItem("Quit", false)) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void RenderLayer::renderGizmo()
{
	Entity selectedEntity = WorldManager::GetLoadedWorld()->GetSelectedEntity();
	if (selectedEntity != -1)
	{
		auto& entityTransform = WorldManager::GetLoadedWorld()->GetComponent<TransformComponent>(selectedEntity);

		ImGuizmo::AllowAxisFlip(false);
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(
			m_ViewportBounds[0].x,
			m_ViewportBounds[0].y,
			m_ViewportBounds[1].x - m_ViewportBounds[0].x,
			m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		auto transform = entityTransform.getTransform();

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
			TransformComponent::DecomposeTransform(transform, translation, rotation, scale);
			glm::vec3 deltaRotation = glm::degrees(rotation) - entityTransform.m_Rotation;

			entityTransform.m_Position = translation;
			entityTransform.m_Rotation += deltaRotation;
			entityTransform.m_Scale = scale;
		}
	}
}

void RenderLayer::renderDebugInfo(float posX, float posY)
{
	float debugMargin = 15.0f;
	ImGui::SetCursorPos({ posX + debugMargin, posY + debugMargin });

	ImGui::SetCursorPos(ImGui::GetCursorPos());
	ImGui::Text("Hovered entity: %d", m_HoveredEntityId);
	ImGui::SetCursorPos({ ImGui::GetCursorPos().x + debugMargin, ImGui::GetCursorPos().y });
	ImGui::Text("Mouse: X: %f Y: %f", m_WindowMousePos.x, m_WindowMousePos.y);
	ImGui::SetCursorPos({ ImGui::GetCursorPos().x + debugMargin, ImGui::GetCursorPos().y });
	ImGui::Text("Is mouse inside window: %s", isMouseInWindow() ? "True" : "False");
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
		WorldManager::Get().GetLoadedWorld()->SetSelectedEntity(-1);
		return false;
	}
}

bool RenderLayer::onMouseButtonPresed(Moongoose::MousePressedEvent& event)
{
	bool mouseHoveredOverEntity = m_HoveredEntityId != -1;
	bool entitySelectButtonsPressed = Input::IsMousePressed(MG_MOUSE_BUTTON_LEFT) && Input::IsKeyPressed(MG_KEY_LEFT_SHIFT);

	if (mouseHoveredOverEntity && entitySelectButtonsPressed) WorldManager::GetLoadedWorld()->SetSelectedEntity(m_HoveredEntityId);

	return false;
}

bool RenderLayer::isMouseInWindow() const
{
	glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	return m_WindowMousePos.x >= 0 && m_WindowMousePos.y >= 0 && m_WindowMousePos.x < (int)viewportSize.x && m_WindowMousePos.y < (int)viewportSize.y;
}
