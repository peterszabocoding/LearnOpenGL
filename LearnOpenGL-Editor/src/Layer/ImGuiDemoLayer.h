#pragma once
#include <LearnOpenGL.h>

class ImGuiDemoLayer : public LearnOpenGL::Layer
{
public:
	ImGuiDemoLayer() = default;
	~ImGuiDemoLayer() override = default;

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(LearnOpenGL::Event& event) override {}
	virtual void onImGuiRender() override;

};
