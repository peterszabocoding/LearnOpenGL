#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Ui/ImGuiLayer.h"
#include "Events/ApplicationEvent.h"

#include "Asset/AssetManager.h"
#include "ECS/WorldManager.h"
#include "Renderer/FramebufferManager.h"

namespace LearnOpenGL
{
	class Application
	{
	public:
		Application();
		virtual ~Application();
		inline static Application& Get() { return *s_Instance; }

		void Run();
		void OnEvent(Event& event);
		void PushLayer(Layer* layer);

		inline Window& GetWindow() const { return *m_Window; }

		FramebufferManager* GetFramebufferManager() const
		{
			return &FramebufferManager::Get();
		}

		AssetManager* GetAssetManager() const
		{
			return &AssetManager::Get();
		}

		WorldManager* GetWorldManager() const
		{
			return &WorldManager::Get();
		}


	private:

		bool OnWindowClosed(WindowCloseEvent& event);

	private:
		static Application* s_Instance;
		bool m_Running = true;

		float m_LastFrameTime = 0.0f;

		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		std::unique_ptr<Window> m_Window;
	};

	Application* CreateApplication();
}

