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

protected:
	virtual bool onWindowResized(Moongoose::WindowResizeEvent& event) override { return false; }

	virtual bool onMouseButtonPresed(Moongoose::MousePressedEvent& event) override { return false; }
	virtual bool onMouseButtonReleased(Moongoose::MouseReleasedEvent& event) override { return false; }

	virtual bool onMouseMoved(Moongoose::MouseMovedEvent& event) override { return false; }
	virtual bool onMouseScrolled(Moongoose::MouseScrolledEvent& event) override { return false; }

	virtual bool onKeyPressed(Moongoose::KeyPressedEvent& event) override { return false; }
	virtual bool onKeyReleased(Moongoose::KeyReleasedEvent& event) override { return false; }
	virtual bool onKeyTyped(Moongoose::KeyTypedEvent& event) override { return false; }

private:

	Moongoose::Framebuffer* m_RenderBuffer;
	Moongoose::PerspectiveCamera* m_EditorCamera;
	Moongoose::Shader* m_BaseShader;

	Moongoose::Mesh m_Mesh;

};

