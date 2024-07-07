#include "EntityInspectorLayer.h"
#include <imgui/imgui.h>
#include "GUI/GuiWidgets.h"

#include "Resource/ResourceManager.h"
#include "Platform/PlatformUtils.h"
#include "Moongoose/Renderer/Material.h"
#include "Moongoose/Renderer/OpenGL/OpenGLTexture2D.h"

using namespace Moongoose;

void EntityInspectorLayer::onImGuiRender()
{
	ImGui::Begin("Entity Inspector");

	Ref<World> world = WorldManager::Get().GetLoadedWorld();
	if (!world)
	{
		ImGui::End();
		return;
	}

	Entity selectedEntity = world->GetSelectedEntity();
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

	ImVec2 windowSize = ImGui::GetWindowSize();

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

				ImGui::SeparatorText("Mesh");

				if (cMesh.m_Mesh)
				{
					RenderImageTextButton(
						ImVec2(50.0f, 50.0f), 
						ResourceManager::GetIcon(Icon::Mesh), 
						cMesh.m_Mesh->GetModelSource());
				}
				else
				{
					ImGui::Text("Empty");
				}

				if (cMesh.m_Mesh)
				{
					ImGui::SeparatorText("Materials");
					auto& materials = cMesh.m_Mesh->GetMaterials();

					for (size_t i = 0; i < materials.size(); i++)
					{
						DrawMaterialControls(cMesh.m_Mesh, i);
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
	const auto& materialSlots = mesh->GetMaterials();
	const auto& materialSlot = materialSlots[materialIndex];
	const auto& material = mesh->GetMaterial(materialIndex);

	if (!material) return;

	Ref<Texture2D> albedo = material->getAlbedo();
	const std::string& matName = material->GetName();

	ImGui::PushID(material->m_ID);
	ImGui::Text("%s:", materialSlots[materialIndex].name.c_str());

	RenderImageTextButton(ImVec2(50.0f, 50.0f), albedo, matName);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
			UUID materialId = ((const AssetDeclaration*)payload->Data)->ID;
			mesh->SetMaterial(
				materialIndex,
				AssetManager::Get().GetAssetById<Material>(materialId)
			);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::PopID();
	ImGui::Separator();
}

void EntityInspectorLayer::RenderImageTextButton(ImVec2 imageSize, Ref<Texture2D> icon, std::string text)
{
	ImGui::BeginGroup();

	auto availSpace = ImGui::GetContentRegionAvail();
	auto& ogPos = ImGui::GetCursorPos();

	ImGui::Image(icon->GetPointerToData(), ImVec2{ 50.0f, 50.0f });
	ImGui::SameLine();
	ImGui::Spacing();
	ImGui::SameLine();

	auto& pos = ImGui::GetCursorPos();

	ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
	pos.y = ogPos.y + textSize.y;

	float text_offset_y = (imageSize.y - textSize.y) * 0.5f;
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + text_offset_y);

	ImGui::Text(text.c_str());

	ImGui::SetCursorPos(ogPos);
	ImGui::Button("", ImVec2(availSpace.x, 50.0f));

	ImGui::EndGroup();
}
