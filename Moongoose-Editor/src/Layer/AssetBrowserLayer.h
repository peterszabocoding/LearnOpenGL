#pragma once
#include <Moongoose.h>

class AssetBrowserLayer : public Moongoose::Layer
{
public:
	AssetBrowserLayer() = default;
	~AssetBrowserLayer() override {}

	virtual void onAttach() override;
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(Moongoose::Event& event) override {}
	virtual void onImGuiRender() override;

private:
	void RenderFolder(Moongoose::FileStructureNode folder, bool openOnStart = false);
	void OnButtonClicked(const Moongoose::UUID id);
	void RenderAssetCard(const Moongoose::AssetDeclaration& decl, const std::function<void(const Moongoose::UUID)>& OnButtonClicked);

private:
	std::filesystem::path m_SelectedFolder;
};
