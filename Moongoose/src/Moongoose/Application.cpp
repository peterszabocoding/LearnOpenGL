#include "mgpch.h"

#include "Log.h"
#include "Application.h"
#include "Moongoose/Events/ApplicationEvent.h"

namespace Moongoose
{
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window->OnUpdate();
		}
	}
}


