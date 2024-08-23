#include <imgui/imgui.h>
#include "EntityInspectorLayer.h"
#include "GUI/GuiWidgets.h"
#include "Moongoose/Renderer/Light.h"
#include "Resource/ResourceManager.h"

using namespace Moongoose;

static uint8_t GetResolutionPositionInList(ShadowMapResolution resolution)
{
	switch (resolution)
	{
	case ShadowMapResolution::ULTRA_LOW:
		return 0;
	case ShadowMapResolution::LOW:
		return 1;
	case ShadowMapResolution::MEDIUM:
		return 2;
	case ShadowMapResolution::HIGH:
		return 3;
	case ShadowMapResolution::VERY_HIGH:
		return 4;
	case ShadowMapResolution::ULTRA_HIGH:
		return 5;
	}
	return 0;
}

static ShadowMapResolution GetResolutionFromPositionInList(uint8_t pos)
{
	switch (pos)
	{
	case 0: return ShadowMapResolution::ULTRA_LOW;
	case 1: return ShadowMapResolution::LOW;
	case 2: return ShadowMapResolution::MEDIUM;
	case 3: return ShadowMapResolution::HIGH;
	case 4: return ShadowMapResolution::VERY_HIGH;
	case 5: return ShadowMapResolution::ULTRA_HIGH;
	}
}

static std::string ShadowMapResolutionToString(ShadowMapResolution resolution)
{
	return std::to_string((uint16_t)resolution);
}

static std::vector<std::string> GetShadowMapResolutionStrings()
{
	return {
		ShadowMapResolutionToString(ShadowMapResolution::ULTRA_LOW),
		ShadowMapResolutionToString(ShadowMapResolution::LOW),
		ShadowMapResolutionToString(ShadowMapResolution::MEDIUM),
		ShadowMapResolutionToString(ShadowMapResolution::HIGH),
		ShadowMapResolutionToString(ShadowMapResolution::VERY_HIGH),
		ShadowMapResolutionToString(ShadowMapResolution::ULTRA_HIGH)
	};
}


static uint8_t GetShadowTypePositionInList(ShadowType shadowType)
{
	switch (shadowType)
	{
	case ShadowType::NONE:
		return 0;
	case ShadowType::HARD:
		return 1;
	case ShadowType::SOFT:
		return 2;
	}
}

static ShadowType GetShadowTypeFromPositionInList(uint8_t pos)
{
	switch (pos)
	{
	case 0: return ShadowType::NONE;
	case 1: return ShadowType::HARD;
	case 2: return ShadowType::SOFT;
	}
}

static std::string ShadowTypeToString(ShadowType shadowType)
{
	switch (shadowType)
	{
	case ShadowType::NONE:
		return "None";
	case ShadowType::HARD:
		return "Hard";
	case ShadowType::SOFT:
		return "Soft";
	}
}

static std::vector<std::string> GetShadowTypeStrings()
{
	return {
		ShadowTypeToString(ShadowType::NONE),
		ShadowTypeToString(ShadowType::HARD),
		ShadowTypeToString(ShadowType::SOFT),
	};
}


void EntityInspectorLayer::onAttach()
{
	m_AssetManager = GetApplication()->GetAssetManager();
	m_WorldManager = GetApplication()->GetWorldManager();
}

void EntityInspectorLayer::onImGuiRender()
{
	ImGui::Begin("Entity Inspector");

	const Ref<World> world = m_WorldManager->GetLoadedWorld();
	if (!world)
	{
		ImGui::End();
		return;
	}

	const Entity selectedEntity = world->GetSelectedEntity();
	if (selectedEntity == -1)
	{
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
		DisplayAddAtmosphericsComponentEntry("Atmospherics Component", selectedEntity);
		ImGui::EndPopup();
	}

	ImVec2 windowSize = ImGui::GetWindowSize();

	// ID & Tag Component
	{
		const IDComponent& cId = world->GetComponent<IDComponent>(selectedEntity);
		TagComponent& cTag = world->GetComponent<TagComponent>(selectedEntity);
		ImGui::Dummy({windowSize.x, 1.0f});

		static char tagArray[50];
		strcpy(tagArray, cTag.Tag.c_str());

		ImGui::Text("ID:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(cId.m_ID).c_str());

		ImGui::Text("Tag:");
		ImGui::SameLine();
		ImGui::InputText("##label", tagArray, IM_ARRAYSIZE(tagArray));

		auto newTag = std::string(tagArray);
		if (newTag != cTag.Tag && !newTag.empty())
		{
			cTag.Tag = std::string(tagArray);
		}

		ImGui::Dummy({windowSize.x, 10.0f});
		ImGui::Separator();
	}

	// Transform Component
	{
		if (world->HasComponent<TransformComponent>(selectedEntity))
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Transform"))
			{
				TransformComponent& cTransform = world->GetComponent<TransformComponent>(selectedEntity);
				GuiWidgets::DrawVec3Control("Position", cTransform.m_Position, 0.0f, windowSize.x / 4);
				GuiWidgets::DrawVec3Control("Rotation", cTransform.m_Rotation, 0.0f, windowSize.x / 4);
				GuiWidgets::DrawVec3Control("Scale", cTransform.m_Scale, 1.0f, windowSize.x / 4);
				ImGui::Dummy({windowSize.x, 10.0f});

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
			if (ImGui::TreeNode("Mesh Renderer"))
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				ImGui::SeparatorText("Mesh");

				RenderImageTextButton(
					ImVec2(50.0f, 50.0f),
					ResourceManager::GetIcon(Icon::Mesh),
					cMesh.m_Mesh ? cMesh.m_Mesh->GetModelSource() : "None");

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET"))
					{
						UUID meshId = ((const AssetDeclaration*)payload->Data)->id;
						cMesh.m_Mesh = m_AssetManager->GetAssetById<Mesh>(meshId);
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
			GuiWidgets::DrawSingleSelectDropdown("Type", Utils::GetLightTypeStrings(), (int)cLight.m_Type,
			                                     [&](int selected)
			                                     {
				                                     cLight.m_Type = (LightType)selected;
			                                     });

			GuiWidgets::DrawFloatControl("Intensity", cLight.m_Intensity, 0.0f, 10000.0f, 0.1f, 1.0f, windowSize.x);
			GuiWidgets::DrawFloatControl("Ambient Intensity", cLight.m_AmbientIntensity, 0.0f, 10000.0f, 0.01f, 1.0f,
			                             windowSize.x);
			GuiWidgets::DrawFloatControl("Attenuation Radius", cLight.m_AttenuationRadius, 0.0f, 10000.0f, 0.1f, 50.0f,
			                             windowSize.x);
			GuiWidgets::DrawFloatControl("Attenuation Angle", cLight.m_AttenuationAngle, 0.0f, 1.0f, 0.005f, 0.75f,
			                             windowSize.x);
			GuiWidgets::DrawRGBColorPicker("Color", cLight.m_Color, 1.0f, windowSize.x);

			GuiWidgets::DrawSingleSelectDropdown("Shadow Type", GetShadowTypeStrings(),
			                                     GetShadowTypePositionInList(cLight.m_ShadowType),
			                                     [&](const int selected)
			                                     {
				                                     cLight.m_ShadowType = GetShadowTypeFromPositionInList(selected);
			                                     });

			GuiWidgets::DrawSingleSelectDropdown("Shadow Map Resolution", GetShadowMapResolutionStrings(),
			                                     GetResolutionPositionInList(cLight.m_ShadowMapResolution),
			                                     [&](const int selected)
			                                     {
				                                     cLight.m_ShadowMapResolution = GetResolutionFromPositionInList(
					                                     selected);
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
			if (ImGui::TreeNode("Billboard"))
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);

				if (world->HasComponent<LightComponent>(selectedEntity))
				{
					LightType type = world->GetComponent<LightComponent>(selectedEntity).m_Type;
					cBillboard.billboardTexture = ResourceManager::GetIcon(type == LightType::Directional
						                                                       ? Icon::DirectionalLight
						                                                       : Icon::PointLight);
				}

				GuiWidgets::DrawRGBColorPicker("Color Tint", cBillboard.tintColor, 1.0f, windowSize.x);

				RenderImageTextButton(
					ImVec2(50.0f, 50.0f),
					cBillboard.billboardTexture ? cBillboard.billboardTexture : ResourceManager::GetIcon(Icon::Texture),
					cBillboard.billboardTexture ? cBillboard.billboardTexture->m_Name : "None");

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET"))
					{
						UUID textureId = ((const AssetDeclaration*)payload->Data)->id;
						cBillboard.billboardTexture = m_AssetManager->GetAssetById<Texture2D>(textureId);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::TreePop();
			}
		}
	}

	// Atmospherics Component
	{
		if (world->HasComponent<AtmosphericsComponent>(selectedEntity))
		{
			auto& cAtmospherics = world->GetComponent<AtmosphericsComponent>(selectedEntity);
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Atmospherics"))
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);

				GuiWidgets::DrawVec3Control("Sun Direction", cAtmospherics.m_SunDirection, 0.0f, windowSize.x / 4);
				GuiWidgets::DrawRGBColorPicker("Sun Color", cAtmospherics.m_SunColor, 1.0f, windowSize.x);
				GuiWidgets::DrawFloatControl("Sun Intensity", cAtmospherics.m_SunIntensity, 0.0f, 10000.0f, 0.1f, 1.0f,
				                             windowSize.x);

				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}

void EntityInspectorLayer::DrawMaterialControls(const Ref<Mesh>& mesh, const unsigned int materialIndex)
{
	const auto& materialSlots = mesh->GetMaterials();
	const auto& [materialName, material] = materialSlots[materialIndex];

	if (!material) return;

	const std::string& matName = material->GetName();

	ImGui::PushID(material->m_Id);
	ImGui::Text("%s:", materialSlots[materialIndex].name.c_str());

	RenderImageTextButton(ImVec2(50.0f, 50.0f),
	                      material->m_Albedo ? material->m_Albedo : ResourceManager::GetIcon(Icon::Material), matName);

	ImGui::PopID();
}

void EntityInspectorLayer::RenderImageTextButton(const ImVec2 imageSize, const Ref<Texture2D>& icon,
                                                 const std::string& text)
{
	ImGui::BeginGroup();

	const auto availSpace = ImGui::GetContentRegionAvail();
	const auto& ogPos = ImGui::GetCursorPos();

	ImGui::Image(icon->GetPointerToData(), ImVec2{50.0f, 50.0f});
	ImGui::SameLine();
	ImGui::Spacing();
	ImGui::SameLine();

	auto pos = ImGui::GetCursorPos();

	const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
	pos.y = ogPos.y + textSize.y;

	const float textOffsetY = (imageSize.y - textSize.y) * 0.5f;
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + textOffsetY);

	ImGui::Text(text.c_str());

	ImGui::SetCursorPos(ogPos);
	ImGui::Button("", ImVec2(availSpace.x, 50.0f));

	ImGui::EndGroup();
}

void EntityInspectorLayer::DisplayAddMeshComponentEntry(const std::string& entryName, size_t entityId) const
{
	const bool hasComponent = m_WorldManager->GetLoadedWorld()->HasComponent<MeshComponent>(entityId);
	if (!hasComponent)
	{
		if (ImGui::MenuItem(entryName.c_str()))
		{
			m_WorldManager->GetLoadedWorld()->AddComponent(entityId, MeshComponent());
			ImGui::CloseCurrentPopup();
		}
	}
}

void EntityInspectorLayer::DisplayAddLightComponentEntry(const std::string& entryName, size_t entityId) const
{
	const bool hasComponent = m_WorldManager->GetLoadedWorld()->HasComponent<LightComponent>(entityId);

	if (!hasComponent)
	{
		if (ImGui::MenuItem(entryName.c_str()))
		{
			m_WorldManager->GetLoadedWorld()->AddComponent(entityId, LightComponent());
			ImGui::CloseCurrentPopup();
		}
	}
}

void EntityInspectorLayer::DisplayAddBillboardComponentEntry(const std::string& entryName, size_t entityId) const
{
	const bool hasComponent = m_WorldManager->GetLoadedWorld()->HasComponent<BillboardComponent>(entityId);
	if (!hasComponent)
	{
		if (ImGui::MenuItem(entryName.c_str()))
		{
			m_WorldManager->GetLoadedWorld()->AddComponent(entityId, BillboardComponent());
			ImGui::CloseCurrentPopup();
		}
	}
}

void EntityInspectorLayer::DisplayAddAtmosphericsComponentEntry(const std::string& entryName, size_t entityId) const
{
	bool hasComponent = m_WorldManager->GetLoadedWorld()->HasComponent<AtmosphericsComponent>(entityId);
	if (!hasComponent)
	{
		if (ImGui::MenuItem(entryName.c_str()))
		{
			m_WorldManager->GetLoadedWorld()->AddComponent(entityId, AtmosphericsComponent());

			if (!m_WorldManager->GetLoadedWorld()->HasComponent<LightComponent>(entityId))
			{
				LightComponent directionalLight;
				directionalLight.m_Type = LightType::Directional;
				m_WorldManager->GetLoadedWorld()->AddComponent(entityId, directionalLight);
			}

			ImGui::CloseCurrentPopup();
		}
	}
}
