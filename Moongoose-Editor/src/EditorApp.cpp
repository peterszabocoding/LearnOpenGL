
#include <Moongoose.h>

class EditorApp : public Moongoose::Application
{
public:
	EditorApp()
	{
		PushOverlay(new Moongoose::ImGuiLayer());
	}
	virtual ~EditorApp() {}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}