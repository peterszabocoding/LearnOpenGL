#include <LearnOpenGL.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <LearnOpenGL/EntryPoint.h>

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
#include "LearnOpenGL/Renderer/ShaderManager.h"

using namespace LearnOpenGL;

class EditorApp : public Application
{
public:
	EditorApp()
	{
		ResourceManager::LoadResources();
		GetAssetManager()->BuildAssetRegistry();

		ShaderManager::AssignShaderToType(ShaderType::STATIC,
		                                  "shader\\shader.vert",
		                                  "shader\\shader.frag", "");

		ShaderManager::AssignShaderToType(ShaderType::GBUFFER,
		                                  "shader\\shader.vert",
		                                  "shader\\gbuffer.fs", "");

		ShaderManager::AssignShaderToType(ShaderType::BILLBOARD,
		                                  "shader\\billboard.vs",
		                                  "shader\\billboard.fs", "");

		ShaderManager::AssignShaderToType(ShaderType::ATMOSPHERE,
		                                  "shader\\atmos_scattering.vs",
		                                  "shader\\atmos_scattering.fs", "");

		ShaderManager::AssignShaderToType(ShaderType::SHADOW_MAP_DIRECTIONAL,
		                                  "shader\\shadow_map.vs",
		                                  "shader\\directional_shadow_map.fs", "");

		ShaderManager::AssignShaderToType(ShaderType::SHADOW_MAP_SPOT,
		                                  "shader\\shadow_map.vs",
		                                  "shader\\spot_shadow_map.fs", "");

		ShaderManager::AssignShaderToType(ShaderType::SHADOW_MAP_POINT,
		                                  "shader\\point_shadow_map.vs",
		                                  "shader\\point_shadow_map.fs",
		                                  "shader\\point_shadow_map.gs");

		ShaderManager::AssignShaderToType(ShaderType::POST_PROCESS_COMBINE,
			"shader\\post_processing.vs",
			"shader\\post_processing_combine.fs", "");

		ShaderManager::AssignShaderToType(ShaderType::POST_PROCESS_SSR,
		                                  "shader\\post_processing.vs",
		                                  "shader\\post_processing_ssr.fs", "");

		ShaderManager::AssignShaderToType(ShaderType::POST_PROCESS_BOX_BLUR,
		                                  "shader\\post_processing.vs",
		                                  "shader\\post_processing_box_blur.fs", "");

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

Application* LearnOpenGL::CreateApplication()
{
	return new EditorApp();
}
