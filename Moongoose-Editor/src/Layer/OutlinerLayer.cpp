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
	const Ref<World> world = WorldManager::Get().GetLoadedWorld();

	if (!world)
	{
		ImGui::End();
		return;
	}

	static size_t itemCurrentIdx = 0;
	itemCurrentIdx = world->GetSelectedEntity();

	const std::vector<std::string>& tags = world->GetSystem<EntityListSystem>()->GetEntityTagList(world);

	if (ImGui::Button("Add Entity")) world->CreateEntity("New Entity " + std::to_string(++m_NewEntityCounter));
	ImGui::SameLine();
	if (ImGui::Button("Delete Entity")) world->DestroyEntity(world->GetSelectedEntity());


	const ImVec2 size = ImGui::GetContentRegionAvail();
	if (ImGui::BeginListBox("##hierarchyList", size))
	{
		for (int n = 0; n < tags.size(); n++)
		{
			const bool isSelected = (itemCurrentIdx == n);

			ImGui::Image(ResourceManager::GetIcon(Icon::EntityCubeWhite)->GetPointerToData(), ImVec2(15.0f, 15.0f));
			ImGui::SameLine();
			if (ImGui::Selectable(tags[n].c_str(), isSelected)) {
				itemCurrentIdx = n;
				world->SetSelectedEntity(itemCurrentIdx);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}

	ImGui::End();
}
