

#include <Moongoose.h>
#include "Moongoose/EntryPoint.h"

#include "Resource/ResourceManager.h"
#include "Layer/ImGuiConfigLayer.h"
#include "Layer/RenderLayer.h"
#include "Layer/PerformanceLayer.h"
#include "Layer/EntityInspectorLayer.h"
#include "Layer/OutlinerLayer.h"
#include "Layer/AssetBrowserLayer.h"
#include "Layer/AssetInspectorLayer.h"
#include "Layer/ImGuiDemoLayer.h"
#include "Layer/FramebufferViewerLayer.h"

class EditorApp : public Moongoose::Application
{
public:
	EditorApp()
	{
		ResourceManager::LoadResources();

		PushLayer(new ImGuiConfigLayer());
		PushLayer(new PerformanceLayer());
		PushLayer(new EntityInspectorLayer());
		PushLayer(new OutlinerLayer());
		PushLayer(new AssetBrowserLayer());
		PushLayer(new AssetInspectorLayer());
		PushLayer(new ImGuiDemoLayer());
		PushLayer(new FramebufferViewerLayer());
		PushLayer(new RenderLayer());
	}

	~EditorApp() override = default;
};

Moongoose::Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}