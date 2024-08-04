#pragma once

#include <Moongoose.h>

class RenderLayer: public Moongoose::Layer
{

public:
	RenderLayer() = default;
	~RenderLayer() override = default;

	virtual void onAttach() override;
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(Moongoose::Event& event) override;
	virtual void onImGuiRender() override;

private:
	void CreateCamera();

	void RenderToolbarMenu() const;
	void RenderGizmo();
	void RenderDebugInfo(float posX, float posY) const;
	void CalculateWindowMousePosition();

	bool IsMouseInWindow() const;
	virtual bool onKeyPressed(Moongoose::KeyPressedEvent& event) override;
	virtual bool onMouseButtonPressed(Moongoose::MousePressedEvent& event) override;

private:
	Moongoose::WorldManager* m_WorldManager;
	Moongoose::AssetManager* m_AssetManager;

	glm::uvec2 m_WindowSize = { 1920, 1080 };
	Ref<Moongoose::PerspectiveCamera> m_EditorCamera;

	size_t m_GizmoMode = 7;
	int m_HoveredEntityId = -1;

	glm::ivec2 m_WindowMousePos = { 0, 0 };
	glm::vec2 m_ViewportBounds[2];
};

