#include "OutlinerLayer.h"
#include "GUI/GuiWidgets.h"
#include "Moongoose/ECS/WorldManager.h"
#include <imgui/imgui.h>

#include "Resource/ResourceManager.h"

using namespace Moongoose;

unsigned int OutlinerLayer::m_NewEntityCounter = 0;

void OutlinerLayer::onImGuiRender()
{
	ImGui::Begin("Outliner");
	const Ref<World> world = GetApplication()->GetWorldManager()->GetLoadedWorld();

	if (!world)
	{
		ImGui::End();
		return;
	}


	const auto tagComponentArray = world->GetComponentArray<TagComponent>();

	if (ImGui::Button("Add Entity"))
	{
		world->CreateEntity("New Entity " + std::to_string(++m_NewEntityCounter));
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete Entity"))
	{
		world->DestroyEntity(world->GetSelectedEntity());
	}


	static size_t itemCurrentIdx = 0;
	itemCurrentIdx = world->GetSelectedEntity();
	if (ImGui::BeginListBox("##hierarchyList", ImGui::GetContentRegionAvail()))
	{
		const auto entityCount = world->GetEntityCount();
		if (itemCurrentIdx >= entityCount) itemCurrentIdx = 0;
		for (uint32_t n = 0; n < entityCount; n++)
		{
			const bool isSelected = (itemCurrentIdx == n);

			ImGui::Image(ResourceManager::GetIcon(Icon::EntityCubeWhite)->GetPointerToData(), ImVec2(15.0f, 15.0f));
			ImGui::SameLine();
			if (ImGui::Selectable(tagComponentArray->GetData(n).Tag.c_str(), isSelected))
			{
				itemCurrentIdx = n;
				world->SetSelectedEntity(itemCurrentIdx);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (isSelected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	ImGui::End();
}
