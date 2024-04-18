#pragma once
#include <Moongoose.h>

class OutlinerLayer : public Moongoose::Layer
{
public:
	OutlinerLayer() {};
	virtual ~OutlinerLayer() {}

	virtual void onAttach() override {};
	virtual void onDetach() override {};
	virtual void onUpdate(float deltaTime) override {};
	virtual void onEvent(Moongoose::Event& event) override {};
	virtual void onImGuiRender() override;

};
