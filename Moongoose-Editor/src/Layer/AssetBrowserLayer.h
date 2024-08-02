#pragma once
#include <Moongoose.h>

class AssetBrowserLayer : public Moongoose::Layer
{
public:
	AssetBrowserLayer() = default;
	~AssetBrowserLayer() override = default;

	virtual void onAttach() override;
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(Moongoose::Event& event) override {}
	virtual void onImGuiRender() override;

private:
	void RenderFolder(const Moongoose::FileStructureNode& folder, bool openOnStart = false);
	void OnButtonClicked(const Moongoose::UUID& id) const;
	void RenderAssetCard(const Moongoose::AssetDeclaration& decl, const std::function<void(const Moongoose::UUID)>& onButtonClicked) const;
	void ShowPopupMenu() const;

private:
	Moongoose::AssetManager* m_AssetManager;
	std::filesystem::path m_SelectedFolder;
};
