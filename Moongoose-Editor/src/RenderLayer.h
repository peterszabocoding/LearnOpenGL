#pragma once

#include <Moongoose.h>

class RenderLayer: public Moongoose::Layer
{

public:
	RenderLayer();
	virtual ~RenderLayer();

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onUpdate(float deltaTime) override;
	virtual void onEvent(Moongoose::Event& event) override;
	virtual void onImGuiRender() override;

private:

	bool onResize(Moongoose::WindowResizeEvent& event);

	Moongoose::Framebuffer* m_RenderBuffer;
	Moongoose::PerspectiveCamera* m_EditorCamera;
	Moongoose::Shader* m_BaseShader;

	Moongoose::Mesh m_Mesh;

};

