#include "InspectorLayer.h"
#include <imgui/imgui.h>
#include "GuiWidgets.h"
#include "Moongoose/ECS/EntityManager.h"

void InspectorLayer::onImGuiRender()
{
	bool hasTransformComponent = Moongoose::EntityManager::Get().hasComponent<Moongoose::TransformComponent>(Moongoose::EntityManager::Get().getEntities()[0]);

	if (hasTransformComponent)
	{
		ImGui::Begin("Inspector");

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

		ImGui::End();
	}

}
