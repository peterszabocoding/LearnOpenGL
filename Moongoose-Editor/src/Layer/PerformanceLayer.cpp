#include "PerformanceLayer.h"
#include <imgui/imgui.h>

PerformanceLayer::PerformanceLayer()
{
}

PerformanceLayer::~PerformanceLayer()
{
}

void PerformanceLayer::onAttach()
{
}

void PerformanceLayer::onDetach()
{
}

void PerformanceLayer::onUpdate(float deltaTime)
{
}

void PerformanceLayer::onEvent(Moongoose::Event& event)
{
}

void PerformanceLayer::onImGuiRender()
{
	ImGui::Begin("Performance");

	auto& io = ImGui::GetIO();

	ImGui::Text("Vendor: %s", (char*)glGetString(GL_VENDOR));
	ImGui::Text("Renderer: %s", (char*)glGetString(GL_RENDERER));
	ImGui::Text("Version: %s", (char*)glGetString(GL_VERSION));
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::End();
}
