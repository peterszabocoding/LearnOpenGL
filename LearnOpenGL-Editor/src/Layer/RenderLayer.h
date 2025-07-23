#pragma once

#include <LearnOpenGL.h>
#include <ImGuizmo.h>

class RenderLayer: public LearnOpenGL::Layer
{

public:
	RenderLayer() = default;
	~RenderLayer() override = default;

	virtual void onAttach() override;
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(LearnOpenGL::Event& event) override;
	virtual void onImGuiRender() override;

private:
	void CreateCamera();

	void RenderToolbarMenu() const;
	void RenderGizmo() const;
	void RenderDebugInfo(float posX, float posY) const;
	void CalculateWindowMousePosition();

	bool IsMouseInWindow() const;
	virtual bool onKeyPressed(LearnOpenGL::KeyPressedEvent& event) override;
	virtual bool onMouseButtonPressed(LearnOpenGL::MousePressedEvent& event) override;

private:
	LearnOpenGL::WorldManager* m_WorldManager;
	LearnOpenGL::AssetManager* m_AssetManager;

	glm::uvec2 m_WindowSize = { 1920, 1080 };
	Ref<LearnOpenGL::PerspectiveCamera> m_EditorCamera;

	ImGuizmo::OPERATION m_GizmoMode = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE m_GizmoTransformMode = ImGuizmo::WORLD;
	int m_HoveredEntityId = -1;

	glm::ivec2 m_WindowMousePos = { 0, 0 };
	glm::vec2 m_ViewportBounds[2];
};

