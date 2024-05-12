#include "AssetBrowserLayer.h"
#include "GuiWidgets.h"
#include <imgui/imgui.h>
#include "Moongoose/Asset/AssetManager.h"

void AssetBrowserLayer::onImGuiRender()
{
	ImGui::Begin("Asset Browser");
	auto& io = ImGui::GetIO();

	static size_t item_current_idx = 0;

	std::vector<std::string> listItems;
	std::vector<Ref<Moongoose::Asset>> assets;

	for (const auto& e : Moongoose::AssetManager::Get().GetLoadedAssets()) {
		auto& [id, asset] = e;
		listItems.push_back(asset->m_Name);
		assets.push_back(asset);
	}

	GuiWidgets::DrawList("##hierarchyList", listItems, item_current_idx, [&](int selected) {
		item_current_idx = selected;
		Moongoose::AssetManager::Get().SetSelectedAsset(assets[selected]);
	});

	ImGui::End();
}
