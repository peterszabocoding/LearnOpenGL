#include "OutlinerLayer.h"
#include "GUI/GuiWidgets.h"
#include "Moongoose/ECS/WorldManager.h"
#include <imgui/imgui.h>

unsigned int OutlinerLayer::m_NewEntityCounter = 0;

void OutlinerLayer::onImGuiRender()
{
	ImGui::Begin("Outliner");
	Ref<Moongoose::World> world = Moongoose::WorldManager::Get().GetLoadedWorld();

	if (!world)
	{
		ImGui::End();
		return;
	}

	auto& io = ImGui::GetIO();

	static size_t item_current_idx = 0;
	item_current_idx = world->GetSelectedEntity();

	auto& tags = world->GetSystem<Moongoose::EntityListSystem>()->GetEntityTagList(world);

	if (ImGui::Button("Add Entity")) world->CreateEntity("New Entity " + std::to_string(++m_NewEntityCounter));
	ImGui::SameLine();
	if (ImGui::Button("Delete Entity")) world->DestroyEntity(world->GetSelectedEntity());


	GuiWidgets::DrawList("##hierarchyList", tags, item_current_idx, [&](int selected) {
		item_current_idx = selected;
		world->SetSelectedEntity(selected);
	});

	ImGui::End();
}
