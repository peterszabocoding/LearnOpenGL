
#include <Moongoose.h>

class EditorApp : public Moongoose::Application
{
public:
	EditorApp()
	{
		Moongoose::LOG_APP_INFO("Post build copy test");
		Moongoose::LOG_APP_ERROR("Error test");
		Moongoose::LOG_APP_WARN("Premake test");
	}
	virtual ~EditorApp()
	{

	}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}