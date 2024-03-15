
#include <Moongoose.h>

class ExampleLayer : public Moongoose::Layer {

public:

	ExampleLayer(): Layer("ExampleLayer") {}

	void onUpdate() override 
	{
		//LOG_APP_INFO("Example layer update");
	}

	void onEvent(Moongoose::Event& event) override
	{
		LOG_APP_WARN("{0}", event);
	}

};

class EditorApp : public Moongoose::Application
{
public:
	EditorApp()
	{
		LOG_APP_INFO("Post build copy test");
		LOG_APP_ERROR("Error test");
		LOG_APP_WARN("Premake test");

		PushLayer(new ExampleLayer());
		PushOverlay(new Moongoose::ImGuiLayer());
	}
	virtual ~EditorApp() {}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}