#include "AssetInspectorLayer.h"
#include <imgui/imgui.h>
#include "GUI/GuiWidgets.h"
#include "Moongoose/Asset/AssetTypes.h"
#include "Moongoose/Asset/AssetManager.h"
#include "Resource/ResourceManager.h"
#include "Platform/PlatformUtils.h"

using namespace Moongoose;

void AssetInspectorLayer::DrawTextureAssetGui(AssetDeclaration& decl, const Ref<Texture2D>& texture) const
{
	ImGui::SeparatorText("Texture Parameters");
	ImGui::Text("Texture ID: %s", std::to_string(texture->m_Id).c_str());
	ImGui::Text("Texture Name: %s", texture->m_Name.c_str());
	ImGui::Text("Texture Type: %s", Utils::TextureTypeToString(texture->getType()));

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Texture File:");
	ImGui::SameLine();
	GuiWidgets::DrawButton(decl.filePath.string(), [&]
	{
			decl.filePath = FileDialogs::OpenFile(".png");
			m_AssetManager->ReloadAsset(decl);
		});

	const std::vector<std::string> textureWrapOptions = {"None", "Clamp", "Repeat"};
	const std::vector<std::string> textureFilterOptions = {"None", "Linear", "Nearest", "Cubic"};

	GuiWidgets::DrawSingleSelectDropdown("Texture Wrap", textureWrapOptions, to_underlying(texture->GetTextureWrap()), [&](int selected) {});
	GuiWidgets::DrawSingleSelectDropdown("Texture Filter", textureFilterOptions, to_underlying(texture->GetTextureFilter()), [&](int selected) {});

	GuiWidgets::DrawTextureImage(texture->GetPointerToData(), ImVec2{ 256.0f, 256.0f });
}

void AssetInspectorLayer::DrawMaterialAssetGui(AssetDeclaration& decl, const Ref<Material>& material) const
{
	constexpr ImVec2 imgSize = { 50.0f, 50.0f };
	const Ref<Texture2D> albedoTexture = material->GetAlbedo();

	ImGui::SeparatorText("Material Parameters");
	ImGui::Text("Shader Type: %s", Utils::GetShaderTypeString(material->GetShaderType()).c_str());

	ImGui::Separator();

	ImGui::PushID(albedoTexture->m_Id);
	RenderImageTextButton(imgSize, albedoTexture, albedoTexture->m_Name);

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
			const UUID textureId = ((const AssetDeclaration*) payload->Data)->id;
			material->SetAlbedo(m_AssetManager->GetAssetById<Texture2D>(textureId));
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::PopID();
}

void AssetInspectorLayer::DrawMeshAssetGui(AssetDeclaration& decl, const Ref<Mesh>& mesh) const
{
	ImGui::SeparatorText("Mesh Parameters");

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Mesh File:");
	ImGui::SameLine();
	GuiWidgets::DrawButton(decl.filePath.string(), [&]()
		{
			decl.filePath = FileDialogs::OpenFile(".fbx");
			m_AssetManager->ReloadAsset(decl);
		}
	);

	std::vector<MaterialSlot> materialSlots = mesh->GetMaterials();

	ImGui::SeparatorText("Materials");
	for (size_t i = 0; i < materialSlots.size(); i++)
	{
		auto& [materialName, material] = materialSlots[i];

		if (!material)
		{
			ImGui::PushID(materialName.c_str());
			ImGui::Text("%s:", materialName.c_str());
			RenderImageTextButton(ImVec2(50.0f, 50.0f), ResourceManager::GetIcon(Icon::Material), "Empty");
		}
		else
		{
			const Ref<Texture2D> albedo = material->GetAlbedo();
			const std::string& matName = material->GetName();

			ImGui::PushID(material->m_Id);
			ImGui::Text("%s:", materialName.c_str());

			RenderImageTextButton(ImVec2(50.0f, 50.0f), albedo, matName);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
				UUID materialId = ((const AssetDeclaration*)payload->Data)->id;
				mesh->SetMaterial(
					i,
					m_AssetManager->GetAssetById<Material>(materialId)
				);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PopID();
	}
}

void AssetInspectorLayer::RenderImageTextButton(const ImVec2 imageSize, const Ref<Texture2D>& icon, const std::string& text)
{
	ImGui::BeginGroup();

	const auto availSpace = ImGui::GetContentRegionAvail();
	const auto& ogPos = ImGui::GetCursorPos();

	ImGui::Image(icon ? icon->GetPointerToData() : ResourceManager::GetIcon(Icon::Texture)->GetPointerToData(), ImVec2{50.0f, 50.0f});
	ImGui::SameLine();
	ImGui::Spacing();
	ImGui::SameLine();

	const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
	const float textOffsetY = (imageSize.y - textSize.y) * 0.5f;

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + textOffsetY);

	ImGui::Text(text.c_str());

	ImGui::SetCursorPos(ogPos);
	ImGui::Button("", ImVec2(availSpace.x, 50.0f));

	ImGui::EndGroup();
}

void AssetInspectorLayer::onAttach()
{
	m_AssetManager = GetApplication()->GetAssetManager();
}

void AssetInspectorLayer::onImGuiRender()
{
	ImGui::Begin("Asset Inspector");

	if (auto& selectedAsset = m_AssetManager->GetSelectedAsset())
	{
		const UUID& assetId = selectedAsset->m_Id;
		AssetDeclaration& assetDecl = m_AssetManager->GetDeclById(assetId);

		GuiWidgets::DrawButton("Save Asset", [&]()
		{
			LOG_APP_INFO("Asset save button clicked");
			m_AssetManager->SaveAsset(selectedAsset);
		});

		ImGui::SeparatorText("Asset Parameters");

		ImGui::Text("ID: %s", std::to_string(assetDecl.id).c_str());
		ImGui::Text("Name: %s", assetDecl.name.c_str());
		ImGui::Text("Type: %s", Utils::AssetTypeToString(assetDecl.type));

		switch (assetDecl.type)
		{
			case AssetType::Mesh:
				DrawMeshAssetGui(assetDecl, CAST_TO(Moongoose::Mesh, selectedAsset));
				break;
			case AssetType::Material:
				DrawMaterialAssetGui(assetDecl, CAST_TO(Moongoose::Material, selectedAsset));
				break;
			case AssetType::Texture:
				DrawTextureAssetGui(assetDecl, CAST_TO(Moongoose::Texture2D, selectedAsset));
				break;
		}

	}

	ImGui::End();
}
