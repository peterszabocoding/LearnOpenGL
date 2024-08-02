#pragma once

#include <Moongoose.h>

class RenderLayer: public Moongoose::Layer
{

public:
	RenderLayer() = default;
	~RenderLayer() override = default;

	virtual void onAttach() override;
	virtual void onDetach() override {};
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(Moongoose::Event& event) override;
	virtual void onImGuiRender() override;

private:
	void CreateRenderBuffer();
	void CreatePreviewRenderBuffer();
	void CreateCamera();

	void RenderToolbarMenu();
	void RenderGizmo();
	void RenderDebugInfo(float posX, float posY) const;

	bool IsMouseInWindow() const;
	virtual bool onKeyPressed(Moongoose::KeyPressedEvent& event) override;
	virtual bool onMouseButtonPressed(Moongoose::MousePressedEvent& event) override;

private:
	Moongoose::WorldManager* m_WorldManager;

	glm::uvec2 m_WindowSize = { 1280, 720 };
	
	Ref<Moongoose::Framebuffer> m_RenderBuffer;
	Ref<Moongoose::Framebuffer> m_PreviewRenderBuffer;
	Ref<Moongoose::PerspectiveCamera> m_EditorCamera;
	Ref<Moongoose::LightSystem> m_LightSystem;
	Ref<Moongoose::RenderSystem> m_RenderSystem;
	Ref<Moongoose::BillboardSystem> m_BillboardSystem;
	Ref<Moongoose::AtmosphericsSystem> m_AtmosphericsSystem;

	size_t m_GizmoMode = 7;
	int m_HoveredEntityId = -1;

	glm::vec2 m_WindowMousePos = { 0, 0 };
	glm::vec2 m_ViewportBounds[2];
};

