#pragma once
#include <Moongoose.h>

class ImGuiDemoLayer : public Moongoose::Layer
{
public:
	ImGuiDemoLayer() = default;
	~ImGuiDemoLayer() override = default;

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(Moongoose::Event& event) override {}
	virtual void onImGuiRender() override;

};
