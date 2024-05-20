#include "AssetBrowserLayer.h"
#include "GuiWidgets.h"
#include <imgui/imgui.h>
#include "Moongoose/Asset/AssetManager.h"
#include "Moongoose/Asset/AssetLoader.h"

void AssetBrowserLayer::onAttach()
{
    auto& assetManager = Moongoose::AssetManager::Get();
    m_IconAssetUnknown = assetManager.LoadAsset<Moongoose::Texture2D>("iconAssetUnknown", "Resource\\icon\\icon_asset_unknown.png");
    m_IconAssetMesh = assetManager.LoadAsset<Moongoose::Texture2D>("iconAssetMesh", "Resource\\icon\\icon_asset_mesh.png");
    m_IconAssetTexture = assetManager.LoadAsset<Moongoose::Texture2D>("iconAssetTexture", "Resource\\icon\\icon_asset_texture.png");
    m_IconAssetMaterial = assetManager.LoadAsset<Moongoose::Texture2D>("iconAssetMaterial", "Resource\\icon\\icon_asset_material.png");
}

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

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 padding = ImVec2{ 5.0f, 5.0f };
    ImVec2 buttonSize = ImVec2{ 120.0f, 150.0f };
    int lineLength = windowSize.x / (buttonSize.x + padding.x);
    if (lineLength <= 0) {
        ImGui::End();
        return;
    }

    for (int n = 0; n < assets.size(); n++)
    {
        float buttonPosX = padding.x + (n % lineLength) * buttonSize.x + (n % lineLength) * padding.x;
        float buttonPosY = 25.0f + padding.y + (n / lineLength) * padding.y + (n / lineLength) * buttonSize.y;

        ImVec2 buttonPos = ImVec2{ buttonPosX, buttonPosY };

        ImGui::PushID(n);
        if ((n % lineLength) != 0) ImGui::SameLine();

        Ref<Moongoose::Texture2D> icon;
        switch (assets[n]->getAssetType())
        {
            case Moongoose::AssetType::Mesh:
                GuiWidgets::ImageButtonWithText(m_IconAssetMesh, assets[n]->m_Name, buttonPos, buttonSize);
                break;
            case Moongoose::AssetType::Texture:
                GuiWidgets::ImageButtonWithText(m_IconAssetTexture, assets[n]->m_Name, buttonPos, buttonSize);
                break;
            case Moongoose::AssetType::Material:
                GuiWidgets::ImageButtonWithText(m_IconAssetMaterial, assets[n]->m_Name, buttonPos, buttonSize);
                break;
            default:
                GuiWidgets::ImageButtonWithText(m_IconAssetUnknown, assets[n]->m_Name, buttonPos, buttonSize);
                break;
        }

        /*
        // Our buttons are both drag sources and drag targets here!
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            // Set payload to carry the index of our item (could be anything)
            ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));

            // Display preview (could be anything, e.g. when dragging an image we could decide to display
            // the filename and a small preview of the image, etc.)
            ImGui::Text("Copy %s", assets[n]->m_Name.c_str());
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
            {
                IM_ASSERT(payload->DataSize == sizeof(int));
                int payload_n = *(const int*)payload->Data;

                listItems[n] = listItems[payload_n];
            }
            ImGui::EndDragDropTarget();
        }
        */

        ImGui::PopID();
    }

	ImGui::End();
}
