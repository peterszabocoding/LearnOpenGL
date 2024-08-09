#include <Moongoose.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <Moongoose/EntryPoint.h>

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
#include "Moongoose/Renderer/ShaderManager.h"

using namespace Moongoose;

class EditorApp : public Application
{
public:
	EditorApp()
	{
		ResourceManager::LoadResources();
		GetAssetManager()->BuildAssetRegistry();

		ShaderManager::AssignShaderToType(ShaderType::STATIC, "shader\\shader.vert", "shader\\shader.frag", "");
		ShaderManager::AssignShaderToType(ShaderType::BILLBOARD, "shader\\billboard.vs", "shader\\billboard.fs", "");
		ShaderManager::AssignShaderToType(ShaderType::ATMOSPHERE, "shader\\atmos_scattering.vs", "shader\\atmos_scattering.fs", "");
		ShaderManager::AssignShaderToType(ShaderType::SHADOW_MAP_DIRECTIONAL, "shader\\shadow_map.vs", "shader\\directional_shadow_map.fs", "");
		ShaderManager::AssignShaderToType(ShaderType::SHADOW_MAP_SPOT,	"shader\\shadow_map.vs", "shader\\spot_shadow_map.fs", "");
		ShaderManager::AssignShaderToType(ShaderType::SHADOW_MAP_POINT,	"shader\\point_shadow_map.vs",	"shader\\point_shadow_map.fs", "shader\\point_shadow_map.gs");

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

Application* Moongoose::CreateApplication()
{
	return new EditorApp();
}