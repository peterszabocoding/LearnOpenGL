#include "AssetInspectorLayer.h"
#include <imgui/imgui.h>
#include "GuiWidgets.h"
#include "Moongoose/Asset/AssetTypes.h"
#include "Moongoose/Asset/AssetManager.h"
#include "Platform/PlatformUtils.h"

static inline void DrawTextureAssetGUI(Moongoose::AssetDeclaration& decl, Ref<Moongoose::Texture2D> texture)
{
	ImGui::SeparatorText("Texture Parameters");
	ImGui::Text("Texture ID: %s", std::to_string(texture->m_ID).c_str());
	ImGui::Text("Texture Name: %s", texture->m_Name.c_str());
	ImGui::Text("Texture Type: %s", Moongoose::Utils::TextureTypeToString(texture->getType()));

	ImGui::Text("Texture File:");
	ImGui::SameLine();
	GuiWidgets::DrawButton(decl.FilePath.string(), [&]()
		{
			decl.FilePath = Moongoose::FileDialogs::OpenFile(".png");
			Moongoose::AssetManager::Get().ReloadAsset(decl);
		});

	std::vector<std::string> textureWrapOptions = {"None", "Clamp", "Repeat"};
	std::vector<std::string> textureFilterOptions = {"None", "Linear", "Nearest", "Cubic"};

	GuiWidgets::DrawSingleSelectDropdown("Texture Wrap", textureWrapOptions, to_underlying(texture->GetTextureWrap()), [&](int selected) {});
	GuiWidgets::DrawSingleSelectDropdown("Texture Filter", textureFilterOptions, to_underlying(texture->GetTextureFilter()), [&](int selected) {});

	GuiWidgets::DrawTextureImage(texture->GetPointerToData(), ImVec2{ 256.0f, 256.0f });
}

static inline void DrawMaterialAssetGUI(Moongoose::AssetDeclaration& decl, Ref<Moongoose::Material> material)
{
	Ref<Moongoose::Texture2D> albedoTexture = material->getAlbedo();

	ImGui::SeparatorText("Material Parameters");
	ImGui::Text("Shader Type: %s", Moongoose::Utils::GetShaderTypeString(material->getShaderType()).c_str());

	ImGui::Separator();
}

void AssetInspectorLayer::onImGuiRender()
{
	ImGui::Begin("Asset Inspector");
	auto& selectedAsset = Moongoose::AssetManager::Get().GetSelectedAsset();

	if (selectedAsset)
	{
		auto& assetId = selectedAsset->m_ID;
		auto& assetDecl = Moongoose::AssetManager::Get().GetDeclByID(assetId);

		GuiWidgets::DrawButton("Save Asset", []()
		{
			LOG_APP_INFO("Asset save button clicked");
		});

		ImGui::SeparatorText("Asset Parameters");

		ImGui::Text("ID: %s", std::to_string(assetDecl.ID).c_str());
		ImGui::Text("Name: %s", assetDecl.Name.c_str());
		ImGui::Text("Type: %s", Moongoose::Utils::AssetTypeToString(assetDecl.Type));

		switch (assetDecl.Type)
		{
			case Moongoose::AssetType::Material:
				DrawMaterialAssetGUI(assetDecl, CastTo<Moongoose::Material>(selectedAsset));
				break;
			case Moongoose::AssetType::Texture:
				DrawTextureAssetGUI(assetDecl, CastTo<Moongoose::Texture2D>(selectedAsset));
				break;
		}

	}

	ImGui::End();
}
