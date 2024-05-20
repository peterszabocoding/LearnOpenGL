#include "EntityInspectorLayer.h"
#include <imgui/imgui.h>
#include "GuiWidgets.h"

#include "Platform/PlatformUtils.h"
#include "Moongoose/Renderer/Material.h"
#include "Moongoose/Renderer/OpenGL/OpenGLTexture2D.h"

using namespace Moongoose;

void EntityInspectorLayer::onImGuiRender()
{
	ImGui::Begin("Entity Inspector");

	auto& loadedWorld = WorldManager::Get().GetLoadedWorld();
	if (!loadedWorld)
	{
		ImGui::End();
		return;
	}

	auto selectedEntity = loadedWorld->GetSelectedEntity();
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

	bool hasTransformComponent = loadedWorld->HasComponent<TransformComponent>(selectedEntity);
	bool hasMeshComponent = loadedWorld->HasComponent<MeshComponent>(selectedEntity);
	bool hasLightComponent = loadedWorld->HasComponent<LightComponent>(selectedEntity);

	if (hasTransformComponent)
	{
		auto& tag = loadedWorld->GetTag(selectedEntity);
		auto& cTransform = loadedWorld->GetComponent<TransformComponent>(selectedEntity);

		auto windowSize = ImGui::GetWindowSize();

		ImGui::Dummy({ windowSize.x , 1.0f });

		static char tagArray[50];
		strcpy(tagArray, tag.c_str());
		ImGui::InputText("##label", tagArray, IM_ARRAYSIZE(tagArray));

		std::string newTag = std::string(tagArray);
		if (newTag != tag) loadedWorld->SetTag(selectedEntity, std::string(tagArray));

		ImGui::Dummy({ windowSize.x , 5.0f });
		ImGui::Separator();

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Transform")) {
			GuiWidgets::DrawVec3Control("Position", cTransform.m_Position, 0.0f, windowSize.x / 4);
			GuiWidgets::DrawVec3Control("Rotation", cTransform.m_Rotation, 0.0f, windowSize.x / 4);
			GuiWidgets::DrawVec3Control("Scale", cTransform.m_Scale, 1.0f, windowSize.x / 4);
			ImGui::Dummy({ windowSize.x , 10.0f });

			ImGui::TreePop();
		}
	}

	if (hasMeshComponent)
	{
		auto& cMesh = loadedWorld->GetComponent<MeshComponent>(selectedEntity);

		if (ImGui::TreeNode("Mesh Renderer")) {
			
			if (ImGui::TreeNode("Mesh"))
			{
				ImGui::Text("Mesh: ");
				ImGui::SameLine();

				if (cMesh.m_Mesh)
				{
					ImGui::Text(cMesh.m_Mesh->GetModelSource().c_str());
				}
				else
				{
					ImGui::Text("Empty");
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Materials")) {
				auto& materials = cMesh.m_Mesh->GetMaterials();
				for (auto& mat : materials)
				{
					DrawMaterialControls(mat);
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}

	if (hasLightComponent)
	{
		auto& cLight = loadedWorld->GetComponent<LightComponent>(selectedEntity);
		auto windowSize = ImGui::GetWindowSize();
		GuiWidgets::DrawFloatControl("Intensity", cLight.m_Intensity, 0.0f, 10000.0f, 0.1f, 1.0f, windowSize.x);
		GuiWidgets::DrawRGBColorPicker("Color", cLight.m_Color, 1.0f, windowSize.x);
	}

	ImGui::End();
}

void EntityInspectorLayer::DrawMaterialControls(Ref<Moongoose::Material> material)
{
	if (!material) return;

	auto albedo = material->getAlbedo();
	auto& matName = material->GetName();
	ImGui::PushID("Material");

	ImGui::Text("Material: %s", matName.c_str());

	ImGui::PopID();
	ImGui::Separator();
}
