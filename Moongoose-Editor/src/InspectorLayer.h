#pragma once
#include <Moongoose.h>

class InspectorLayer : public Moongoose::Layer
{
public:
	InspectorLayer() {};
	virtual ~InspectorLayer() {};

	virtual void onAttach() override {};
	virtual void onDetach() override {};
	virtual void onUpdate(float deltaTime) override {};
	virtual void onEvent(Moongoose::Event& event) override {};
	virtual void onImGuiRender() override;

};
