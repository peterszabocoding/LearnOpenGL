#include "OutlinerLayer.h"
#include "GUI/GuiWidgets.h"
#include "Moongoose/ECS/WorldManager.h"
#include <imgui/imgui.h>

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


	GuiWidgets::DrawList("##hierarchyList", tags, itemCurrentIdx, [&](const int selected) {
		itemCurrentIdx = selected;
		world->SetSelectedEntity(selected);
	});

	ImGui::End();
}
