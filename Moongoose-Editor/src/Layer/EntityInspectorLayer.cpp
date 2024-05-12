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

	auto selectedEntity = EntityManager::Get().getSelectedEntity();
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

	bool hasTransformComponent = EntityManager::Get().hasComponent<TransformComponent>(EntityManager::Get().getEntities()[selectedEntity]);
	bool hasMeshComponent = EntityManager::Get().hasComponent<MeshComponent>(EntityManager::Get().getEntities()[selectedEntity]);
	bool hasLightComponent = EntityManager::Get().hasComponent<LightComponent>(EntityManager::Get().getEntities()[selectedEntity]);

	
	if (hasTransformComponent)
	{
		auto& tag = EntityMemoryPool::Get().getTag(selectedEntity);
		auto& cTransform = EntityManager::Get().getComponent<TransformComponent>(selectedEntity);

		auto windowSize = ImGui::GetWindowSize();

		ImGui::Dummy({ windowSize.x , 1.0f });

		static char tagArray[50];
		strcpy(tagArray, tag.c_str());
		ImGui::InputText("##label", tagArray, IM_ARRAYSIZE(tagArray));

		std::string newTag = std::string(tagArray);
		if (newTag != tag) EntityManager::Get().setTag(selectedEntity, std::string(tagArray));

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
		auto& cMesh = EntityManager::Get().getComponent<MeshComponent>(selectedEntity);

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
		auto& cLight = EntityManager::Get().getComponent<LightComponent>(selectedEntity);
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

	if (albedo) {
		ImGui::Text("Albedo Map: ");
		ImGui::SameLine();
		GuiWidgets::DrawTextureImage(albedo->getPointerToData(), ImVec2{ 128.0f, 128.0f });
	}

	ImGui::PopID();
	ImGui::Separator();
}
