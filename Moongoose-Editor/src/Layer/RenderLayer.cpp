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
	m_WorldManager = GetApplication()->GetWorldManager();
	m_AssetManager = GetApplication()->GetAssetManager();

	CreateCamera();
}

void RenderLayer::CalculateWindowMousePosition()
{
	auto [mx, my] = ImGui::GetMousePos();
	mx -= m_ViewportBounds[0].x;
	my -= m_ViewportBounds[0].y;
	const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	my = viewportSize.y - my;
	m_WindowMousePos = { mx, my };
}

void RenderLayer::onUpdate(const float deltaTime)
{
	if (!m_WorldManager->isWorldOpened()) return;

	m_EditorCamera->SetCameraActive(IsMouseInWindow());
	m_EditorCamera->OnUpdate(deltaTime);

	Renderer::RenderWorld(m_EditorCamera, m_WorldManager->GetLoadedWorld());

	CalculateWindowMousePosition();

	if (IsMouseInWindow())
	{
		Renderer::GetRenderBuffer()->Bind();
		m_HoveredEntityId = Renderer::GetRenderBuffer()->ReadPixel(1, m_WindowMousePos.x, m_WindowMousePos.y);
		Renderer::GetRenderBuffer()->Unbind();
	}
}

void RenderLayer::onEvent(Event& event)
{
	m_EditorCamera->OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MousePressedEvent>(BIND_EVENT_FUNC(RenderLayer::onMouseButtonPressed));
	dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FUNC(RenderLayer::onKeyPressed));
}

void RenderLayer::onImGuiRender()
{
	RenderToolbarMenu();

	ImGuizmo::BeginFrame();
	ImGui::Begin("Render");

	if (!m_WorldManager->IsWorldOpened())
	{
		ImGui::End();
		return;
	}

	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	if (windowSize.x != m_WindowSize.x || windowSize.y != m_WindowSize.y)
	{
		m_WindowSize = { windowSize.x, windowSize.y };
		m_EditorCamera->SetRenderResolution(m_WindowSize.x, m_WindowSize.y);
	}

	const ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
	const ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	const ImVec2 viewportOffset = ImGui::GetWindowPos();

	m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	if (Renderer::GetRenderBuffer())
	{
		ImGui::Image((void*)Renderer::GetRenderBuffer()->GetColorAttachments()[0],
			windowSize,
			ImVec2(0, 1),
			ImVec2(1, 0));

		RenderGizmo();
		RenderDebugInfo(viewportMinRegion.x, viewportMinRegion.y);
	}

	ImGui::End();
}

void RenderLayer::CreateCamera()
{
	PerspectiveCamera::Params params;
	params.renderWidth = m_WindowSize.x;
	params.renderHeight = m_WindowSize.y;
	params.startPosition = { 0.0f, 0.0f, 5.0f };
	m_EditorCamera = CreateRef<PerspectiveCamera>(params);
}

void RenderLayer::RenderToolbarMenu() const
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
				const std::string worldFilePath = FileDialogs::OpenFile(".mgworld");
				if (!worldFilePath.empty()) m_WorldManager->LoadWorld(worldFilePath);
			}

			if (ImGui::MenuItem("Save World", "", false, m_WorldManager->IsWorldOpened()))
			{
				const std::string worldName = m_WorldManager->GetLoadedWorld()->m_Name;
				m_WorldManager->SaveWorld("Content\\Worlds\\" + worldName + ".mgworld");
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Quit", nullptr)) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void RenderLayer::RenderGizmo()
{
	const Entity selectedEntity = WorldManager::GetLoadedWorld()->GetSelectedEntity();
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

		auto transform = entityTransform.GetTransform();

		Manipulate(
			value_ptr(m_EditorCamera->GetViewMatrix()),
			value_ptr(m_EditorCamera->GetProjection()),
			ImGuizmo::OPERATION(m_GizmoMode),
			ImGuizmo::LOCAL,
			value_ptr(transform),
			nullptr,
			nullptr);

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, rotation, scale;
			TransformComponent::DecomposeTransform(transform, translation, rotation, scale);
			const glm::vec3 deltaRotation = degrees(rotation) - entityTransform.m_Rotation;

			entityTransform.m_Position = translation;
			entityTransform.m_Rotation += deltaRotation;
			entityTransform.m_Scale = scale;
		}
	}
}

void RenderLayer::RenderDebugInfo(const float posX, const float posY) const
{
	constexpr float debugMargin = 15.0f;
	ImGui::SetCursorPos({ posX + debugMargin, posY + debugMargin });

	ImGui::SetCursorPos(ImGui::GetCursorPos());
	ImGui::Text("Hovered entity: %d", m_HoveredEntityId);
	ImGui::SetCursorPos({ ImGui::GetCursorPos().x + debugMargin, ImGui::GetCursorPos().y });
	ImGui::Text("Mouse: X: %f Y: %f", m_WindowMousePos.x, m_WindowMousePos.y);
	ImGui::SetCursorPos({ ImGui::GetCursorPos().x + debugMargin, ImGui::GetCursorPos().y });
	ImGui::Text("Is mouse inside window: %s", IsMouseInWindow() ? "True" : "False");
}

bool RenderLayer::onKeyPressed(KeyPressedEvent& event)
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
		m_WorldManager->GetLoadedWorld()->SetSelectedEntity(-1);
		return false;
	}

	return false;
}

bool RenderLayer::onMouseButtonPressed(MousePressedEvent& event)
{
	//const bool mouseHoveredOverEntity = m_HoveredEntityId != -1;
	const bool entitySelectButtonsPressed = Input::IsMousePressed(MG_MOUSE_BUTTON_LEFT) && Input::IsKeyPressed(MG_KEY_LEFT_SHIFT);

	if (WorldManager::GetLoadedWorld() && IsMouseInWindow() && entitySelectButtonsPressed)
	{
		WorldManager::GetLoadedWorld()->SetSelectedEntity(m_HoveredEntityId);
	}
	return false;
}

bool RenderLayer::IsMouseInWindow() const
{
	const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	return m_WindowMousePos.x >= 0 && m_WindowMousePos.y >= 0 && m_WindowMousePos.x < (int)viewportSize.x && m_WindowMousePos.y < (int)viewportSize.y;
}
