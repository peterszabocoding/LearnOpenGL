
#include <Moongoose.h>
#include <imgui.h>

class ExampleLayer : public Moongoose::Layer
{
public:
	ExampleLayer() : Layer("ExampleLayer") {}
	~ExampleLayer() {}

	virtual void onUpdate() override
	{
	}

};

class EditorApp : public Moongoose::Application
{
public:
	EditorApp()
	{
		PushLayer(new ExampleLayer());
		PushGuiLayer(new Moongoose::ImGuiLayer());
	}
	virtual ~EditorApp() {}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}