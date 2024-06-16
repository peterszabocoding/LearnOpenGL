#pragma once
#include <Moongoose.h>

class AssetBrowserLayer : public Moongoose::Layer
{
public:
	AssetBrowserLayer() {};
	virtual ~AssetBrowserLayer() {};

	virtual void onAttach() override;
	virtual void onDetach() override {};
	virtual void onUpdate(float deltaTime) override {};
	virtual void onEvent(Moongoose::Event& event) override {};
	virtual void onImGuiRender() override;

private:
	void OnButtonClicked(const char* id);

private:
	Ref<Moongoose::Texture2D> m_IconAssetUnknown;
	Ref<Moongoose::Texture2D> m_IconAssetMesh;
	Ref<Moongoose::Texture2D> m_IconAssetTexture;
	Ref<Moongoose::Texture2D> m_IconAssetMaterial;

};
