#pragma once
#include <Moongoose.h>

class FramebufferViewerLayer : public Moongoose::Layer
{
public:
	FramebufferViewerLayer() {}
	virtual ~FramebufferViewerLayer() {}

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(Moongoose::Event& event) override {}
	virtual void onImGuiRender() override;

private:
	int selectedFramebuffer = 0;
	int selectedAttachment = 0;

};
