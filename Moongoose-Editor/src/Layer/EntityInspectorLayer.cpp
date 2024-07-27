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
		DisplayAddLightComponentEntry("Light Component", selectedEntity);
		DisplayAddMeshComponentEntry("Mesh Component", selectedEntity);
		DisplayAddBillboardComponentEntry("Billboard Component", selectedEntity);
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
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Transform")) {
				TransformComponent& cTransform = world->GetComponent<TransformComponent>(selectedEntity);
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
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Mesh Renderer")) {
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				ImGui::SeparatorText("Mesh");

				RenderImageTextButton(
					ImVec2(50.0f, 50.0f),
					ResourceManager::GetIcon(Icon::Mesh),
					cMesh.m_Mesh ? cMesh.m_Mesh->GetModelSource() : "None");

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
						UUID meshId = ((const AssetDeclaration*)payload->Data)->ID;
						cMesh.m_Mesh = AssetManager::Get().GetAssetById<Mesh>(meshId);
					}
					ImGui::EndDragDropTarget();
				}

				if (cMesh.m_Mesh)
				{
					ImGui::SeparatorText("Materials");
					auto& materials = cMesh.m_Mesh->GetMaterials();

					for (size_t i = 0; i < materials.size(); i++)
					{
						DrawMaterialControls(cMesh.m_Mesh, i);
						ImGui::Spacing();
					}
				}

				ImGui::TreePop();
			}
		}
	}

	// Light Component
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (world->HasComponent<LightComponent>(selectedEntity) && ImGui::TreeNode("Light"))
		{
			auto& cLight = world->GetComponent<LightComponent>(selectedEntity);
			auto windowSize = ImGui::GetWindowSize();
			GuiWidgets::DrawFloatControl("Intensity", cLight.m_Intensity, 0.0f, 10000.0f, 0.1f, 1.0f, windowSize.x);
			GuiWidgets::DrawFloatControl("Attenuation Radius", cLight.m_AttenuationRadius, 0.0f, 10000.0f, 0.1f, 50.0f, windowSize.x);
			GuiWidgets::DrawFloatControl("Attenuation Angle", cLight.m_AttenuationAngle, 0.0f, 1.0f, 0.005f, 0.75f, windowSize.x);
			GuiWidgets::DrawRGBColorPicker("Color", cLight.m_Color, 1.0f, windowSize.x);
			GuiWidgets::DrawSingleSelectDropdown("Type", Utils::GetLightTypeStrings(), (int) cLight.m_Type, [&](int selected)
				{
					cLight.m_Type = (LightType) selected;
				});

			ImGui::TreePop();
		}
	}

	// Billboard Component
	{
		if (world->HasComponent<BillboardComponent>(selectedEntity))
		{
			auto& cBillboard = world->GetComponent<BillboardComponent>(selectedEntity);
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Billboard")) {
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);

				if (world->HasComponent<LightComponent>(selectedEntity))
				{
					LightType type = world->GetComponent<LightComponent>(selectedEntity).m_Type;
					cBillboard.m_BillboardTexture = ResourceManager::GetIcon(type == LightType::DIRECTIONAL 
						? Icon::DirectionalLight 
						: Icon::PointLight);
				}

				RenderImageTextButton(
					ImVec2(50.0f, 50.0f),
					cBillboard.m_BillboardTexture ? cBillboard.m_BillboardTexture : ResourceManager::GetIcon(Icon::Texture),
					cBillboard.m_BillboardTexture ? cBillboard.m_BillboardTexture->m_Name : "None");

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
						UUID textureId = ((const AssetDeclaration*)payload->Data)->ID;
						cBillboard.m_BillboardTexture = AssetManager::Get().GetAssetById<Texture2D>(textureId);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}

void EntityInspectorLayer::DrawMaterialControls(Ref<Mesh> mesh, unsigned int materialIndex)
{
	const auto& materialSlots = mesh->GetMaterials();
	const auto& materialSlot = materialSlots[materialIndex];
	const auto& material = materialSlot.material;

	if (!material) return;

	Ref<Texture2D> albedo = material->getAlbedo();
	const std::string& matName = material->GetName();

	ImGui::PushID(material->m_ID);
	ImGui::Text("%s:", materialSlots[materialIndex].name.c_str());

	RenderImageTextButton(ImVec2(50.0f, 50.0f), albedo, matName);

	ImGui::PopID();
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

void EntityInspectorLayer::DisplayAddMeshComponentEntry(const std::string& entryName, size_t entityId)
{
	bool hasComponent = WorldManager::Get().GetLoadedWorld()->HasComponent<MeshComponent>(entityId);

	if (!hasComponent)
	{
		if (ImGui::MenuItem(entryName.c_str()))
		{
			WorldManager::Get().GetLoadedWorld()->AddComponent(entityId, MeshComponent());
			ImGui::CloseCurrentPopup();
		}
	}
}

void EntityInspectorLayer::DisplayAddLightComponentEntry(const std::string& entryName, size_t entityId)
{
	bool hasComponent = WorldManager::Get().GetLoadedWorld()->HasComponent<LightComponent>(entityId);

	if (!hasComponent)
	{
		if (ImGui::MenuItem(entryName.c_str()))
		{
			WorldManager::Get().GetLoadedWorld()->AddComponent(entityId, LightComponent());
			ImGui::CloseCurrentPopup();
		}
	}
}

void EntityInspectorLayer::DisplayAddBillboardComponentEntry(const std::string& entryName, size_t entityId)
{
	bool hasComponent = WorldManager::Get().GetLoadedWorld()->HasComponent<BillboardComponent>(entityId);

	if (!hasComponent)
	{
		if (ImGui::MenuItem(entryName.c_str()))
		{
			WorldManager::Get().GetLoadedWorld()->AddComponent(entityId, BillboardComponent(NULL));
			ImGui::CloseCurrentPopup();
		}
	}
}
