#include "OutlinerLayer.h"
#include "GuiWidgets.h"
#include "Moongoose/ECS/WorldManager.h"
#include <imgui/imgui.h>

void OutlinerLayer::onImGuiRender()
{
	ImGui::Begin("Outliner");

	Ref<Moongoose::World> loadedWorld = Moongoose::WorldManager::Get().GetLoadedWorld();

	if (!loadedWorld)
	{
		ImGui::End();
		return;
	}

	auto& io = ImGui::GetIO();

	static size_t item_current_idx = 0;
	item_current_idx = loadedWorld->GetSelectedEntity();

	std::vector<std::string> listItems;
	for (const auto& e : loadedWorld->GetEntities()) {
		listItems.push_back(loadedWorld->GetTag(e));
	}

	if (ImGui::Button("Add Entity")) 
	{
		loadedWorld->CreateEntity("Entity");
	}

	GuiWidgets::DrawList("##hierarchyList", listItems, item_current_idx, [&](int selected) {
		item_current_idx = selected;
		loadedWorld->SetSelectedEntity(selected);
	});

	ImGui::End();
}
