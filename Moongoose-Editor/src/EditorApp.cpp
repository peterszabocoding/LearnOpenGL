

#include <Moongoose.h>
#include "Moongoose/EntryPoint.h"

#include "Layer/RenderLayer.h"
#include "Layer/PerformanceLayer.h"
#include "Layer/InspectorLayer.h"
#include "Layer/OutlinerLayer.h"

class EditorApp : public Moongoose::Application
{
public:
	EditorApp(): Application()
	{
		PushLayer(new RenderLayer());
		PushLayer(new PerformanceLayer());
		PushLayer(new InspectorLayer());
		PushLayer(new OutlinerLayer());
	}
	virtual ~EditorApp() {}
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}