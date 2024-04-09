

#include <Moongoose.h>
#include "Moongoose/EntryPoint.h"
#include <imgui/imgui.h>

#include "RenderLayer.h"
#include "PerformanceLayer.h"

class EditorApp : public Moongoose::Application
{
public:
	EditorApp(): Application()
	{
		PushLayer(new RenderLayer());
		PushLayer(new PerformanceLayer());
	}
	virtual ~EditorApp() {}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}