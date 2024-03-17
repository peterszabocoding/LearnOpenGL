
#include <Moongoose.h>

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
		PushOverlay(new ExampleLayer());
		PushOverlay(new Moongoose::ImGuiLayer());
	}
	virtual ~EditorApp() {}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}