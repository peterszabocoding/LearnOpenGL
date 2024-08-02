#include "AssetBrowserLayer.h"
#include "GUI/GuiWidgets.h"
#include <imgui/imgui.h>
#include "Platform/PlatformUtils.h"
#include "Resource/ResourceManager.h"

#define BIND_FUNC(x) std::bind(&x, this, std::placeholders::_1)

using namespace Moongoose;

void AssetBrowserLayer::ShowPopupMenu() const
{
    if (ImGui::BeginPopupContextWindow("asset_browser_popup"))
    {
        if (ImGui::MenuItem("Import Mesh"))
        {
            FileDialogs::OpenFile(".fbx", [&](const std::string& filePath)
            {
                 const std::string fileName = std::filesystem::path(filePath).filename().string();
                 const std::string extension = std::filesystem::path(filePath).extension().string();
                 AssetType assetType = s_AssetExtensionMap[extension];

                 if (assetType != AssetType::Mesh) return;

                 
                 AssetDeclaration decl = m_AssetManager->CreateAssetDeclaration<Mesh>(fileName, filePath);
                 const Ref<Mesh> asset = m_AssetManager->LoadAsset<Mesh>(decl);
                 m_AssetManager->SaveAsset(asset);

                 FileSystem::GetFileStructure("Content\\", true);
                 m_AssetManager->BuildAssetRegistry();
            });
		}
        if (ImGui::MenuItem("Import Texture"))
        {
            FileDialogs::OpenFile(".png", [&](const std::string& filePath)
            {
                const std::string fileName = std::filesystem::path(filePath).filename().string();
                const std::string extension = std::filesystem::path(filePath).extension().string();
                AssetType assetType = s_AssetExtensionMap[extension];

                if (assetType != AssetType::Texture) return;

                AssetDeclaration decl = m_AssetManager->CreateAssetDeclaration<Texture2D>(fileName, filePath);
                const Ref<Texture2D> asset = m_AssetManager->LoadAsset<Texture2D>(decl);
                m_AssetManager->SaveAsset(asset);

                FileSystem::GetFileStructure("Content\\", true);
                m_AssetManager->BuildAssetRegistry();
            });
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Create Material"))
        {
	        const auto material = m_AssetManager->CreateAsset<Material>("New_Material", m_SelectedFolder.string() + "\\" + "new_material.mgasset");
            m_AssetManager->SaveAsset(material);

            FileSystem::GetFileStructure("Content\\", true);
            m_AssetManager->BuildAssetRegistry();
        }
        
		ImGui::EndPopup();
	}
}

void AssetBrowserLayer::onAttach()
{
    m_AssetManager = GetApplication()->GetAssetManager();
}

void AssetBrowserLayer::onImGuiRender()
{
	ImGui::Begin("Asset Browser");

    ShowPopupMenu();

    // Folder structure layout
	{
        const ImVec2 availableSpace = ImGui::GetContentRegionAvail();
        ImGui::BeginChild("ChildL", ImVec2(std::max(250.0f, availableSpace.x * 0.15f), availableSpace.y),true);
       
        RenderFolder(FileSystem::GetFileStructure("Content\\"), true);

        ImGui::EndChild();
	}

    ImGui::SameLine();

    // Asset cards
	{
        const ImVec2 availableSpace = ImGui::GetContentRegionAvail();
        constexpr auto cardPadding = ImVec2{ 5.0f, 5.0f };
        constexpr auto cardSize = ImVec2{ 120.0f, 150.0f };

        ImGui::BeginChild("ChildR", ImVec2(availableSpace.x, availableSpace.y), true);
        if (!m_SelectedFolder.empty() && is_directory(m_SelectedFolder))
        {
            if (availableSpace.x > 0) {
                std::vector<AssetDeclaration> decls = m_AssetManager->GetAssetsByFolder(m_SelectedFolder);
                int columns = availableSpace.x / (cardSize.x + cardPadding.x);
                if (columns < 1) columns = 1;

                if (ImGui::BeginTable("CardTable", columns, ImGuiTableFlags_SizingStretchSame)) {
                    for (const auto& decl : decls) {
                        ImGui::TableNextColumn();
                        ImGui::PushID(decl.ID);
                        RenderAssetCard(decl, BIND_FUNC(AssetBrowserLayer::OnButtonClicked));

                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            ImGui::SetDragDropPayload("ASSET", &decl, sizeof(AssetDeclaration));
                            ImGui::Text("Dragging %s", decl.Name.c_str());
                            ImGui::EndDragDropSource();
                        }

                        ImGui::PopID();
                        ImGui::Spacing();
                    }
                    ImGui::EndTable();
                }
            }
        }
        ImGui::EndChild();
	}

	ImGui::End();
}

void AssetBrowserLayer::OnButtonClicked(const UUID& id) const
{
    m_AssetManager->SetSelectedAsset(m_AssetManager->GetAssetById(id));
}

void AssetBrowserLayer::RenderAssetCard(const AssetDeclaration& decl, const std::function<void(const UUID)>& onButtonClicked) const
{
    constexpr auto cardSize = ImVec2{ 120.0f, 150.0f };
    constexpr ImVec2 imgPadding = ImVec2{ 20.0f, 20.0f };
    constexpr ImVec2 imgSize = { cardSize.x - 4 * imgPadding.x, cardSize.x - 4 * imgPadding.y };

	ImGui::BeginGroup();
    auto originalCursorPos = ImGui::GetCursorPos();

    // Render card image

    const Ref<Texture2D> icon = ResourceManager::GetIcon(Icon(decl.Type));
    const ImVec2 imgCursorPos = { originalCursorPos.x + 2 * imgPadding.x, originalCursorPos.y + 2 * imgPadding.y };
    ImGui::SetCursorPos(imgCursorPos);
    ImGui::Image(icon->GetPointerToData(), imgSize);

    // Render card text
    const ImVec2 textSize = ImGui::CalcTextSize(decl.Name.c_str());
    const ImVec2 textCursorPos = { originalCursorPos.x + (cardSize.x - textSize.x) * 0.5f, originalCursorPos.y + cardSize.y - 1.5f * imgPadding.y };
    ImGui::SetCursorPos(textCursorPos);
    ImGui::Text(decl.Name.c_str());

    // Render card button
    ImGui::SetCursorPos(originalCursorPos);
    if (ImGui::Button("", cardSize))
    {
        onButtonClicked(decl.ID);
    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        LOG_APP_INFO("Right clicked on asset card");
        ImGui::OpenPopup("asset_browser_card_popup");
    }

    if (ImGui::BeginPopupContextWindow("asset_browser_card_popup"))
    {
        if (ImGui::MenuItem("Delete Asset"))
        {
            LOG_APP_INFO("Delete asset: {0}", decl.Name);
        }
        if (ImGui::MenuItem("Rename Asset"))
        {
            m_AssetManager->RenameAsset(decl.ID, "New_Asset_Renamed");
            FileSystem::GetFileStructure("Content\\", true);
        }
        ImGui::EndPopup();
    }
    
    ImGui::EndGroup();
}

void AssetBrowserLayer::RenderFolder(const FileStructureNode& folder, const bool openOnStart)
{
    if (!folder.isDirectory) return;

    ImGuiTreeNodeFlags flags = (folder.path == m_SelectedFolder) ? ImGuiTreeNodeFlags_Selected : 0;
    flags |= ImGuiTreeNodeFlags_OpenOnArrow;

    ImGui::SetNextItemOpen(openOnStart, ImGuiCond_Once);
    const bool opened = ImGui::TreeNodeEx(folder.path.string().c_str(), flags);

    if (ImGui::IsItemClicked())
    {
        m_SelectedFolder = folder.path;
    }

    if (opened)
    {
        for (auto& child : folder.children)
        {
            if (!child.isDirectory) continue;
            RenderFolder(child);
        }
        ImGui::TreePop();
    }
}