#pragma once
#include <LearnOpenGL.h>

class AssetBrowserLayer : public LearnOpenGL::Layer
{
public:
	AssetBrowserLayer() = default;
	~AssetBrowserLayer() override = default;

	virtual void onAttach() override;
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(LearnOpenGL::Event& event) override {}
	virtual void onImGuiRender() override;

private:
	void RenderFolder(const LearnOpenGL::FileStructureNode& folder, bool openOnStart = false);
	void OnButtonClicked(const LearnOpenGL::UUID& id) const;
	void RenderAssetCard(const LearnOpenGL::AssetDeclaration& decl, const std::function<void(const LearnOpenGL::UUID)>& onButtonClicked) const;
	void ShowPopupMenu() const;

private:
	LearnOpenGL::AssetManager* m_AssetManager;
	std::filesystem::path m_SelectedFolder;
};
