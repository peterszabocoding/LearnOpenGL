#pragma once
#include <LearnOpenGL.h>

class FramebufferViewerLayer : public LearnOpenGL::Layer
{
public:
	FramebufferViewerLayer() = default;
	~FramebufferViewerLayer() override {}

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(LearnOpenGL::Event& event) override {}
	virtual void onImGuiRender() override;

private:
	int selectedFramebuffer = 0;
	int selectedAttachment = 0;

};
