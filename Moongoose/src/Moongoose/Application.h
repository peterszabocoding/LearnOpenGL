#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Moongoose/Events/ApplicationEvent.h"

namespace Moongoose
{
	class MOONGOOSE_API Application
	{
	public:
		Application();
		virtual ~Application();
		inline static Application& Get() { return *s_Instance; }

		void Run();

		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }

	private:

		bool OnWindowClosed(WindowCloseEvent& event);

	private:
		static Application* s_Instance;
		bool m_Running = true;

		LayerStack m_LayerStack;
		std::unique_ptr<Window> m_Window;
	};

	Application* CreateApplication();
}

