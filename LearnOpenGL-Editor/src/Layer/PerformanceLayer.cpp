#include "PerformanceLayer.h"

#include <backends/imgui_impl_opengl3_loader.h>
#include <imgui/imgui.h>

void PerformanceLayer::onImGuiRender()
{
	ImGui::Begin("Performance");

	const ImGuiIO& io = ImGui::GetIO();

	ImGui::Text("Vendor: %s", (char*)glGetString(GL_VENDOR));
	ImGui::Text("Renderer: %s", (char*)glGetString(GL_RENDERER));
	ImGui::Text("Version: %s", (char*)glGetString(GL_VERSION));
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::End();
}
