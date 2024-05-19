#include "AssetInspectorLayer.h"
#include <imgui/imgui.h>
#include "GuiWidgets.h"
#include "Moongoose/Asset/AssetTypes.h"
#include "Moongoose/Asset/AssetManager.h"

static inline void DrawTextureAssetGUI(Ref<Moongoose::Texture2D> texture)
{
	ImGui::SeparatorText("Texture Parameters");
	ImGui::Text("Texture ID: %s", std::to_string((uint64_t)texture->m_ID).c_str());
	ImGui::Text("Texture Name: %s", texture->m_Name.c_str());
	ImGui::Text("Texture Type: %s", Moongoose::Utils::TextureTypeToString(texture->getType()));

	std::vector<std::string> textureWrapOptions = {"None", "Clamp", "Repeat"};
	std::vector<std::string> textureFilterOptions = {"None", "Linear", "Nearest", "Cubic"};

	GuiWidgets::DrawSingleSelectDropdown("Texture Wrap", textureWrapOptions, to_underlying(texture->getTextureWrap()), [&](int selected) {});
	GuiWidgets::DrawSingleSelectDropdown("Texture Filter", textureFilterOptions, to_underlying(texture->getTextureFilter()), [&](int selected) {});

	GuiWidgets::DrawTextureImage(texture->getPointerToData(), ImVec2{ 128.0f, 128.0f });
}

static inline void DrawMaterialAssetGUI(Ref<Moongoose::Material> material)
{
	Ref<Moongoose::Texture2D> albedoTexture = material->getAlbedo();


	ImGui::SeparatorText("Material Parameters");
	ImGui::Text("Shader Type: %s", Moongoose::Utils::GetShaderTypeString(material->getShaderType()).c_str());

	ImGui::Separator();

	if (albedoTexture) {
		ImGui::Text("Albedo: ");
		DrawTextureAssetGUI(albedoTexture);
	}
}

void AssetInspectorLayer::onImGuiRender()
{
	ImGui::Begin("Asset Inspector");
	auto& selectedAsset = Moongoose::AssetManager::Get().GetSelectedAsset();

	if (selectedAsset)
	{
		auto& assetId = selectedAsset->m_ID;
		auto& assetDecl = Moongoose::AssetManager::Get().GetDeclByID(assetId);

		if (ImGui::Button("Save Asset"))
		{

		}

		ImGui::SeparatorText("Asset Parameters");

		ImGui::Text("ID: %s", std::to_string((uint64_t)assetDecl.ID).c_str());
		ImGui::Text("Name: %s", assetDecl.Name.c_str());
		ImGui::Text("Type: %s", Moongoose::Utils::AssetTypeToString(assetDecl.Type));
		ImGui::Text("Source path: %s", assetDecl.FilePath.string().c_str());

		switch (assetDecl.Type)
		{
		case Moongoose::AssetType::Material:
			DrawMaterialAssetGUI(CastTo<Moongoose::Material>(selectedAsset));
			break;
		case Moongoose::AssetType::Texture:
			DrawTextureAssetGUI(CastTo<Moongoose::Texture2D>(selectedAsset));
			break;
		}

	}

	ImGui::End();
}
