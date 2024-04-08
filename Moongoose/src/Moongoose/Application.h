#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Ui/ImGuiLayer.h"
#include "Events/ApplicationEvent.h"

namespace Moongoose
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

		inline Window& GetWindow() { return *m_Window; }

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

