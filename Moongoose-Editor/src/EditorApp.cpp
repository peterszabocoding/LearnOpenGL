
#include <Moongoose.h>
#include <imgui/imgui.h>

class ExampleLayer : public Moongoose::Layer
{
public:
	ExampleLayer() : Layer("ExampleLayer") {}
	~ExampleLayer() {}

	virtual void onUpdate() override
	{
	}

};

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
		PushLayer(new ExampleLayer());
		PushLayer(new ExampleGuiLayer());
	}
	virtual ~EditorApp() {}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}