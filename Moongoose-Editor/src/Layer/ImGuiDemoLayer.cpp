#include "ImGuiDemoLayer.h"
#include <imgui/imgui.h>

void ImGuiDemoLayer::onImGuiRender()
{
	bool show = true;
	ImGui::ShowDemoWindow(&show);
}