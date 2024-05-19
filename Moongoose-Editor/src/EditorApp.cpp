

#include <Moongoose.h>
#include "Moongoose/EntryPoint.h"

#include "Layer/ImGuiConfigLayer.h"
#include "Layer/RenderLayer.h"
#include "Layer/PerformanceLayer.h"
#include "Layer/EntityInspectorLayer.h"
#include "Layer/OutlinerLayer.h"
#include "Layer/AssetBrowserLayer.h"
#include "Layer/AssetInspectorLayer.h"
#include "Layer/ImGuiDemoLayer.h"

class EditorApp : public Moongoose::Application
{
public:
	EditorApp(): Application()
	{
		PushLayer(new ImGuiConfigLayer());
		PushLayer(new RenderLayer());
		PushLayer(new PerformanceLayer());
		PushLayer(new EntityInspectorLayer());
		PushLayer(new OutlinerLayer());
		PushLayer(new AssetBrowserLayer());
		PushLayer(new AssetInspectorLayer());
		//PushLayer(new ImGuiDemoLayer());
	}
	virtual ~EditorApp() {}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}