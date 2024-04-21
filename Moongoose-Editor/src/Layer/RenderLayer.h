#pragma once

#include <Moongoose.h>

class RenderLayer: public Moongoose::Layer
{

public:
	RenderLayer() {};
	virtual ~RenderLayer() {};

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(Moongoose::Event& event) override;
	virtual void onImGuiRender() override;

private:
	void createRenderBuffer();
	void createCamera();

	bool onKeyPressed(Moongoose::KeyPressedEvent& event);
	virtual bool onMouseButtonPresed(Moongoose::MousePressedEvent& event) override;
	bool isMouseInWindow() const;

private:
	glm::vec2 m_WindowSize = { 1280, 720 };
	
	Scope<Moongoose::Framebuffer> m_RenderBuffer;
	Ref<Moongoose::PerspectiveCamera> m_EditorCamera;

	Ref<Moongoose::RenderSystem> m_RenderSystem;

	Ref<Moongoose::Scene> m_Scene;
	Ref<Moongoose::Shader> m_BaseShader = nullptr;

	Ref<Moongoose::Texture2D> m_CheckerTexture = nullptr;
	Ref<Moongoose::Texture2D> m_ColorCheckerTexture = nullptr;

	size_t m_GizmoMode = 7;
	int m_HoveredEntityId = -1;

	glm::vec2 m_WindowMousePos = { 0, 0 };
	glm::vec2 m_ViewportBounds[2];
};

