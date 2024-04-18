#include "InspectorLayer.h"
#include <imgui/imgui.h>
#include "GuiWidgets.h"
#include "Moongoose/ECS/EntityManager.h"
#include "Moongoose/Renderer/Material.h"
#include "Moongoose/Renderer/OpenGL/OpenGLTexture2D.h"

using namespace Moongoose;

void InspectorLayer::onImGuiRender()
{
	bool hasTransformComponent = EntityManager::Get().hasComponent<TransformComponent>(EntityManager::Get().getEntities()[0]);
	bool hasMeshComponent = EntityManager::Get().hasComponent<MeshComponent>(EntityManager::Get().getEntities()[0]);

	auto selectedEntity = EntityManager::Get().getSelectedEntity();

	ImGui::Begin("Inspector");
	if (hasTransformComponent)
	{
		auto& tag = EntityMemoryPool::Get().getTag(selectedEntity);
		auto& cTransform = EntityManager::Get().getComponent<TransformComponent>(selectedEntity);

		auto windowSize = ImGui::GetWindowSize();

		ImGui::Dummy({ windowSize.x , 1.0f });

		ImGui::Text(tag.c_str());

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
		DrawMaterialControls(cMesh.m_Material);
	}

	ImGui::End();
}

void InspectorLayer::DrawMaterialControls(Ref<Moongoose::Material> material)
{
	auto albedo = material->Albedo;

	ImGui::PushID("Material");
	if (ImGui::TreeNode("Material ID")) {

		ImGui::Text("Albedo Map: ");
		ImGui::SameLine();

		if (albedo) {
			GuiWidgets::DrawTextureImage(albedo->getPointerToData(), ImVec2{ 128.0f, 128.0f });
		}

		ImGui::TreePop();
	}
	ImGui::PopID();

	ImGui::Separator();
}