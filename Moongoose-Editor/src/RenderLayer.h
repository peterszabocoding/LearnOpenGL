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

private:
	glm::vec2 m_WindowSize = { 1280, 720 };
	
	Scope<Moongoose::Framebuffer> m_RenderBuffer;
	Scope<Moongoose::PerspectiveCamera> m_EditorCamera;

	Moongoose::Shader* m_BaseShader = nullptr;
	Moongoose::Mesh* m_Mesh = nullptr;
	Ref<Moongoose::Texture2D> m_CheckerTexture = nullptr;

};

