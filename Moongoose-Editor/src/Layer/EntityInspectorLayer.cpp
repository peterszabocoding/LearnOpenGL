#include "EntityInspectorLayer.h"
#include <imgui/imgui.h>
#include "GUI/GuiWidgets.h"

#include "Platform/PlatformUtils.h"
#include "Moongoose/Renderer/Material.h"
#include "Moongoose/Renderer/OpenGL/OpenGLTexture2D.h"

using namespace Moongoose;

void EntityInspectorLayer::onImGuiRender()
{
	ImGui::Begin("Entity Inspector");

	auto& world = WorldManager::Get().GetLoadedWorld();
	if (!world)
	{
		ImGui::End();
		return;
	}

	auto selectedEntity = world->GetSelectedEntity();
	if (selectedEntity == -1) {
		ImGui::End();
		return;
	}

	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("Add Component");
	}

	if (ImGui::BeginPopup("Add Component"))
	{
		DisplayAddComponentEntry<LightComponent>("Light Component", selectedEntity);
		DisplayAddComponentEntry<MeshComponent>("Mesh Component", selectedEntity);
		ImGui::EndPopup();
	}

	auto windowSize = ImGui::GetWindowSize();

	IDComponent& cID = world->GetComponent<IDComponent>(selectedEntity);
	TagComponent& cTag = world->GetComponent<TagComponent>(selectedEntity);

	// ID & Tag Component
	{
		ImGui::Dummy({ windowSize.x , 1.0f });

		static char tagArray[50];
		strcpy(tagArray, cTag.Tag.c_str());
		
		ImGui::Text("ID:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(cID.m_ID).c_str());

		ImGui::Text("Tag:");
		ImGui::SameLine();
		ImGui::InputText("##label", tagArray, IM_ARRAYSIZE(tagArray));

		std::string newTag = std::string(tagArray);
		if (newTag != cTag.Tag && newTag != "") cTag.Tag = std::string(tagArray);

		ImGui::Dummy({ windowSize.x , 10.0f });
		ImGui::Separator();
	}

	// Transform Component
	{
		if (world->HasComponent<TransformComponent>(selectedEntity))
		{
			TransformComponent& cTransform = world->GetComponent<TransformComponent>(selectedEntity);

			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Transform")) {
				GuiWidgets::DrawVec3Control("Position", cTransform.m_Position, 0.0f, windowSize.x / 4);
				GuiWidgets::DrawVec3Control("Rotation", cTransform.m_Rotation, 0.0f, windowSize.x / 4);
				GuiWidgets::DrawVec3Control("Scale", cTransform.m_Scale, 1.0f, windowSize.x / 4);
				ImGui::Dummy({ windowSize.x , 10.0f });

				ImGui::TreePop();
			}
		}
	}

	// Mesh Component
	{
		if (world->HasComponent<MeshComponent>(selectedEntity))
		{
			auto& cMesh = world->GetComponent<MeshComponent>(selectedEntity);

			if (ImGui::TreeNode("Mesh Renderer")) {

				if (ImGui::TreeNode("Mesh"))
				{
					ImGui::Text("Mesh: ");
					ImGui::SameLine();

					if (cMesh.m_Mesh)
					{
						ImGui::Text(cMesh.m_Mesh->GetModelSource().c_str());

						auto list = AssetManager::Get().GetAssetDeclByType<Mesh>();
						std::vector<std::string> listNames;

						for (const auto& [assetName, decl] : list) listNames.push_back(assetName);

						GuiWidgets::DrawSingleSelectDropdown("Mesh Asset:", listNames, 0, [&](int selected)
						{
								cMesh.m_Mesh = std::static_pointer_cast<Mesh>(AssetManager::Get().GetAssetById(list[selected].second.ID));
						});
					}
					else
					{
						ImGui::Text("Empty");
					}
					ImGui::TreePop();
				}

				if (cMesh.m_Mesh)
				{
					if (ImGui::TreeNode("Materials")) {
						auto& materials = cMesh.m_Mesh->GetMaterials();

						for (size_t i = 0; i < materials.size(); i++)
						{
							DrawMaterialControls(cMesh.m_Mesh, i);
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
	}

	// Light Component
	{
		if (world->HasComponent<LightComponent>(selectedEntity))
		{
			auto& cLight = world->GetComponent<LightComponent>(selectedEntity);
			auto windowSize = ImGui::GetWindowSize();
			GuiWidgets::DrawFloatControl("Intensity", cLight.m_Intensity, 0.0f, 10000.0f, 0.1f, 1.0f, windowSize.x);
			GuiWidgets::DrawRGBColorPicker("Color", cLight.m_Color, 1.0f, windowSize.x);
		}
	}

	ImGui::End();
}

void EntityInspectorLayer::DrawMaterialControls(Ref<Mesh> mesh, unsigned int materialIndex)
{
	const auto material = mesh->GetMaterial(materialIndex);

	if (!material) return;

	auto albedo = material->getAlbedo();
	auto& matName = material->GetName();
	ImGui::PushID("Material" + materialIndex);
	ImGui::Text("Material: %s", matName.c_str());

	auto list = AssetManager::Get().GetAssetDeclByType<Material>();
	std::vector<std::string> listNames;

	for (const auto& [assetName, decl] : list) listNames.push_back(assetName);

	GuiWidgets::DrawSingleSelectDropdown("Material Asset:", listNames, 0, [&](int selected)
		{
			const auto selectedMatAsset = std::static_pointer_cast<Material>(AssetManager::Get().GetAssetById(list[selected].second.ID));
			mesh->SetMaterial(materialIndex, selectedMatAsset);
		});

	ImGui::PopID();
	ImGui::Separator();
}
