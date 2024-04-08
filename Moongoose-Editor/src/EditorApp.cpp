

#include <Moongoose.h>
#include "Moongoose/EntryPoint.h"
#include <imgui/imgui.h>

#include "RenderLayer.h"

class ExampleGuiLayer : public Moongoose::Layer
{
public:
	ExampleGuiLayer(): Layer("ExampleGuiLayer") {}
	~ExampleGuiLayer() {}

	virtual void onImGuiRender() override
	{
		ImGui::Begin("ExampleGuiLayer");
		ImGui::Text("ExampleGuiLayer");
		ImGui::End();
	}
};

class EditorApp : public Moongoose::Application
{
public:
	EditorApp(): Application()
	{
		PushLayer(new ExampleGuiLayer());
		PushLayer(new RenderLayer());
	}
	virtual ~EditorApp() {}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}