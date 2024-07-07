#pragma once
#include <Moongoose.h>

#define CAST_TO(type, x) std::static_pointer_cast<type>(x)

class Asset;

class AssetInspectorLayer : public Moongoose::Layer
{
public:
	AssetInspectorLayer() {};
	virtual ~AssetInspectorLayer() {};

	virtual void onAttach() override {};
	virtual void onDetach() override {};
	virtual void onUpdate(float deltaTime) override {};
	virtual void onEvent(Moongoose::Event& event) override {};
	virtual void onImGuiRender() override;

};
