#include "OutlinerLayer.h"
#include "GuiWidgets.h"
#include "Moongoose/ECS/EntityManager.h"
#include <imgui/imgui.h>

void OutlinerLayer::onImGuiRender()
{
	ImGui::Begin("Outliner");

	auto& io = ImGui::GetIO();

	static size_t item_current_idx = 0;
	item_current_idx = Moongoose::EntityManager::Get().getSelectedEntity();

	std::vector<std::string> listItems;
	for (const auto& e : Moongoose::EntityManager::Get().getEntities()) {
		listItems.push_back(Moongoose::EntityManager::Get().getTag(e));
	}

	if (ImGui::Button("Add Entity")) 
	{
		Moongoose::EntityManager::Get().addEntity("Entity");
	}

	GuiWidgets::DrawList("##hierarchyList", listItems, item_current_idx, [&](int selected) {
		item_current_idx = selected;
		Moongoose::EntityManager::Get().setSelectedEntity(Moongoose::EntityManager::Get().getEntities()[selected]);
	});

	ImGui::End();
}
