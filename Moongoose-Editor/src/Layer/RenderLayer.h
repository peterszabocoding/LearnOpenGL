#pragma once

#include <Moongoose.h>
#include <Moongoose/ECS/World.h>

class RenderLayer: public Moongoose::Layer
{

public:
	RenderLayer() = default;
	virtual ~RenderLayer() {};

	virtual void onAttach() override;
	virtual void onDetach() override {};
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(Moongoose::Event& event) override;
	virtual void onImGuiRender() override;

private:
	void createRenderBuffer();
	void createPreviewRenderBuffer();
	void createCamera();

	void renderToolbarMenu();
	void renderGizmo();
	void renderDebugInfo(float posX, float posY);
	void renderFramebufferPreviewWindow();

	bool onKeyPressed(Moongoose::KeyPressedEvent& event);
	bool isMouseInWindow() const;
	virtual bool onMouseButtonPresed(Moongoose::MousePressedEvent& event) override;

private:
	glm::vec2 m_WindowSize = { 1280, 720 };
	
	Ref<Moongoose::Framebuffer> m_RenderBuffer;
	Ref<Moongoose::Framebuffer> m_PreviewRenderBuffer;
	Ref<Moongoose::PerspectiveCamera> m_EditorCamera;
	Ref<Moongoose::LightSystem> m_LightSystem;
	Ref<Moongoose::RenderSystem> m_RenderSystem;
	Ref<Moongoose::BillboardSystem> m_BillboardSystem;
	Ref<Moongoose::AtmosphericsSystem> m_AtmosphericsSystem;

	double m_Time = 0;

	size_t m_GizmoMode = 7;
	int m_HoveredEntityId = -1;

	int selectedFramebuffer = 0;
	int selectedAttachment = 0;

	glm::vec2 m_WindowMousePos = { 0, 0 };
	glm::vec2 m_ViewportBounds[2];
};

