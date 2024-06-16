#include "AssetBrowserLayer.h"
#include "GuiWidgets.h"
#include <imgui/imgui.h>
#include "Moongoose/Asset/AssetManager.h"
#include "Moongoose/Asset/AssetLoader.h"

#define BIND_FUNC(x) std::bind(&x, this, std::placeholders::_1)

void AssetBrowserLayer::onAttach()
{
    auto& assetManager = Moongoose::AssetManager::Get();
    m_IconAssetUnknown = assetManager.CreateAsset<Moongoose::Texture2D>("iconAssetUnknown", "res\\icon\\icon_asset_unknown.png");
    m_IconAssetMesh = assetManager.CreateAsset<Moongoose::Texture2D>("iconAssetMesh", "res\\icon\\icon_asset_mesh.png");
    m_IconAssetTexture = assetManager.CreateAsset<Moongoose::Texture2D>("iconAssetTexture", "res\\icon\\icon_asset_texture.png");
    m_IconAssetMaterial = assetManager.CreateAsset<Moongoose::Texture2D>("iconAssetMaterial", "res\\icon\\icon_asset_material.png");
}

void AssetBrowserLayer::onImGuiRender()
{
	ImGui::Begin("Asset Browser");
	auto& io = ImGui::GetIO();

	static size_t item_current_idx = 0;

	std::vector<std::string> listItems;
	std::vector<Moongoose::AssetDeclaration> decls;

	for (const auto& e : Moongoose::AssetManager::Get().GetAssetRegistry()) {
		auto& [id, decl] = e;
		listItems.push_back(decl.Name);
        decls.push_back(decl);
	}

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 padding = ImVec2{ 5.0f, 5.0f };
    ImVec2 buttonSize = ImVec2{ 120.0f, 150.0f };
    int lineLength = windowSize.x / (buttonSize.x + padding.x);
    if (lineLength <= 0) {
        ImGui::End();
        return;
    }

    for (int n = 0; n < decls.size(); n++)
    {
        float buttonPosX = padding.x + (n % lineLength) * buttonSize.x + (n % lineLength) * padding.x;
        float buttonPosY = 25.0f + padding.y + (n / lineLength) * padding.y + (n / lineLength) * buttonSize.y;

        ImVec2 buttonPos = ImVec2{ buttonPosX, buttonPosY };

        ImGui::PushID(n);
        if ((n % lineLength) != 0) ImGui::SameLine();

        std::string buttonId = std::to_string(decls[n].ID);

        Ref<Moongoose::Texture2D> icon;
        switch (decls[n].Type)
        {
			case Moongoose::AssetType::Mesh:
				icon = m_IconAssetMesh;
                break;
            case Moongoose::AssetType::Texture:
                icon = m_IconAssetTexture;
                break;
            case Moongoose::AssetType::Material:
                icon = m_IconAssetMaterial;
                break;
            default:
                icon = m_IconAssetUnknown;
                break;
        }

        GuiWidgets::ImageButtonWithText(buttonId.c_str(), icon, decls[n].Name, buttonPos, buttonSize, BIND_FUNC(AssetBrowserLayer::OnButtonClicked));

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

void AssetBrowserLayer::OnButtonClicked(const char* id)
{
    Moongoose::AssetManager::Get().SetSelectedAsset(Moongoose::AssetManager::Get().GetAssetById(Moongoose::UUID(std::stoull(std::string(id)))));
}
