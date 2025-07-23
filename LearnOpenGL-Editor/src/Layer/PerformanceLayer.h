#pragma once
#include <LearnOpenGL.h>

class PerformanceLayer : public LearnOpenGL::Layer
{
public:
	PerformanceLayer() = default;
	~PerformanceLayer() override = default;

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(LearnOpenGL::Event& event) override {}
	virtual void onImGuiRender() override;

};
