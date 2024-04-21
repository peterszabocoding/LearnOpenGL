#pragma once
#include <Moongoose.h>

class PerformanceLayer : public Moongoose::Layer
{
public:
	PerformanceLayer() {}
	virtual ~PerformanceLayer() {}

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(Moongoose::Event& event) override {}
	virtual void onImGuiRender() override;

};
