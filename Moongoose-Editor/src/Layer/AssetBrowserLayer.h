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
	void RenderFolderHierarchy(Moongoose::FileStructureNode root);
	void RenderFolder(Moongoose::FileStructureNode folder);
	void OnButtonClicked(const Moongoose::UUID id);
	void RenderAssetCard(const Moongoose::AssetDeclaration& decl, const std::function<void(const Moongoose::UUID)>& OnButtonClicked);

private:
	std::filesystem::path m_SelectedFolder;
	std::unordered_map<Moongoose::AssetType, Ref<Moongoose::Texture2D>> m_AssetTypeTextures;
};
