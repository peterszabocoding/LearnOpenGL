#pragma once
#include <Moongoose.h>

class AssetBrowserLayer : public Moongoose::Layer
{
public:
	AssetBrowserLayer() {};
	virtual ~AssetBrowserLayer() {};

	virtual void onAttach() override {};
	virtual void onDetach() override {};
	virtual void onUpdate(float deltaTime) override {};
	virtual void onEvent(Moongoose::Event& event) override {};
	virtual void onImGuiRender() override;

private:

};
