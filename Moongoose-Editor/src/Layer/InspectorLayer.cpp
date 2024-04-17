#include "InspectorLayer.h"
#include <imgui/imgui.h>
#include "GuiWidgets.h"
#include "Moongoose/ECS/EntityManager.h"
#include "Moongoose/Renderer/Material.h"
#include "Moongoose/Renderer/OpenGL/OpenGLTexture2D.h"

void InspectorLayer::onImGuiRender()
{
	bool hasTransformComponent = Moongoose::EntityManager::Get().hasComponent<Moongoose::TransformComponent>(Moongoose::EntityManager::Get().getEntities()[0]);
	bool hasMeshComponent = Moongoose::EntityManager::Get().hasComponent<Moongoose::MeshComponent>(Moongoose::EntityManager::Get().getEntities()[0]);

	ImGui::Begin("Inspector");
	if (hasTransformComponent)
	{
		auto& tag = Moongoose::EntityMemoryPool::Get().getTag(Moongoose::EntityManager::Get().getEntities()[0]);
		auto& cTransform = Moongoose::EntityManager::Get().getComponent<Moongoose::TransformComponent>(Moongoose::EntityManager::Get().getEntities()[0]);

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
		auto& cMesh = Moongoose::EntityManager::Get().getComponent<Moongoose::MeshComponent>(Moongoose::EntityManager::Get().getEntities()[0]);
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
			GuiWidgets::DrawTextureImage(albedo->getPointerToData(), ImVec2{128.0f, 128.0f });
		}

		ImGui::TreePop();
	}
	ImGui::PopID();

	ImGui::Separator();
}