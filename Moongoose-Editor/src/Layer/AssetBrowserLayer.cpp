#include "AssetBrowserLayer.h"
#include "GUI/GuiWidgets.h"
#include <imgui/imgui.h>
#include "Platform/PlatformUtils.h"
#include "Resource/ResourceManager.h"

#define BIND_FUNC(x) std::bind(&x, this, std::placeholders::_1)

using namespace Moongoose;

void AssetBrowserLayer::onAttach()
{
}

void AssetBrowserLayer::onImGuiRender()
{
	ImGui::Begin("Asset Browser");

    // Import buttons
	{
        GuiWidgets::DrawButton("Import Mesh", []()
            {
                const std::string filePath = Moongoose::FileDialogs::OpenFile(".fbx");
                const std::string fileName = std::filesystem::path(filePath).filename().string();
                const std::string extension = std::filesystem::path(filePath).extension().string();
                Moongoose::AssetType assetType = Moongoose::s_AssetExtensionMap[extension];

                if (assetType != Moongoose::AssetType::Mesh) return;

                Ref<Moongoose::Mesh> asset = Moongoose::AssetManager::Get().CreateAsset<Moongoose::Mesh>(fileName, filePath);
                Moongoose::AssetManager::Get().SaveAsset(asset);
            });
        ImGui::SameLine();
        GuiWidgets::DrawButton("Import Texture", []()
            {
                std::string filePath = Moongoose::FileDialogs::OpenFile(".png");
                const std::string fileName = std::filesystem::path(filePath).filename().string();
                std::string extension = std::filesystem::path(filePath).extension().string();
                Moongoose::AssetType assetType = Moongoose::s_AssetExtensionMap[extension];

                if (assetType != Moongoose::AssetType::Mesh) return;

                Ref<Moongoose::Texture2D> asset = Moongoose::AssetManager::Get().CreateAsset<Moongoose::Texture2D>(fileName, filePath);
                Moongoose::AssetManager::Get().SaveAsset(asset);
            });
	}

    // Folder structure layout
	{
        const ImVec2 availableSpace = ImGui::GetContentRegionAvail();
        ImGui::BeginChild("ChildL", ImVec2(std::max(250.0f, availableSpace.x * 0.15f), availableSpace.y),true);
        RenderFolderHierarchy(Moongoose::FileSystem::GetFileStructure("Content\\"));
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
            if (availableSpace.x <= 0) {
                ImGui::EndChild();
            }
            else
            {
                std::vector<AssetDeclaration> decls = AssetManager::Get().GetAssetsByFolder(m_SelectedFolder);
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

void AssetBrowserLayer::OnButtonClicked(const UUID id)
{
    Moongoose::AssetManager::Get().SetSelectedAsset(AssetManager::Get().GetAssetById(id));
}

void AssetBrowserLayer::RenderAssetCard(const AssetDeclaration& decl, const std::function<void(const UUID)>& onButtonClicked)
{
    constexpr auto cardSize = ImVec2{ 120.0f, 150.0f };
    ImGui::BeginGroup();
   
    auto& originalCursorPos = ImGui::GetCursorPos();
    constexpr ImVec2 imgPadding = ImVec2{ 20.0f, 20.0f };

    // Render card image
    const ImVec2 imgSize = { cardSize.x - 4 * imgPadding.x, cardSize.x - 4 * imgPadding.y };
    Ref<Texture2D> icon = ResourceManager::GetIcon(Icon(decl.Type));
    ImVec2 imgCursorPos = { originalCursorPos.x + 2 * imgPadding.x, originalCursorPos.y + 2 * imgPadding.y };
    ImGui::SetCursorPos(imgCursorPos);
    ImGui::Image(icon->GetPointerToData(), imgSize);

    // Render card text
    const ImVec2 textSize = ImGui::CalcTextSize(decl.Name.c_str());
    ImVec2 textCursorPos = { originalCursorPos.x + (cardSize.x - textSize.x) * 0.5f, originalCursorPos.y + cardSize.y - 1.5f * imgPadding.y };
    ImGui::SetCursorPos(textCursorPos);
    ImGui::Text(decl.Name.c_str());

    // Render card button
    ImGui::SetCursorPos(originalCursorPos);
    if (ImGui::Button("", cardSize))
    {
        onButtonClicked(decl.ID);
    }
    ImGui::EndGroup();
}

void AssetBrowserLayer::RenderFolderHierarchy(Moongoose::FileStructureNode root)
{
    RenderFolder(root);
}

void AssetBrowserLayer::RenderFolder(Moongoose::FileStructureNode folder)
{
    if (!folder.isDirectory) return;

    ImGuiTreeNodeFlags flags = (folder.path == m_SelectedFolder) ? ImGuiTreeNodeFlags_Selected : 0;
    flags |= ImGuiTreeNodeFlags_OpenOnArrow;

    bool opened = ImGui::TreeNodeEx(folder.path.string().c_str(), flags);
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