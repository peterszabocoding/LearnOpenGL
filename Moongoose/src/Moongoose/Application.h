#pragma once

#include "Core.h"
#include "Window.h"
#include "Moongoose/Events/ApplicationEvent.h"

namespace Moongoose
{
	class MOONGOOSE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& event);

	private:
		bool OnWindowClosed(WindowCloseEvent& event);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	Application* CreateApplication();
}

