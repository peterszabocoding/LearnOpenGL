

#include <Moongoose.h>
#include "Moongoose/EntryPoint.h"
#include <imgui/imgui.h>

#include "RenderLayer.h"
#include "PerformanceLayer.h"
#include "InspectorLayer.h"

class EditorApp : public Moongoose::Application
{
public:
	EditorApp(): Application()
	{
		PushLayer(new RenderLayer());
		PushLayer(new PerformanceLayer());
		PushLayer(new InspectorLayer());
	}
	virtual ~EditorApp() {}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}