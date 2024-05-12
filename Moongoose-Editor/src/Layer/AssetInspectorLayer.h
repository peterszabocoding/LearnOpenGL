#pragma once
#include <Moongoose.h>

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

private:

	template <typename T>
	inline Ref<T> CastTo(Ref<Moongoose::Asset> asset)
	{
		return std::static_pointer_cast<T>(asset);
	}

};
