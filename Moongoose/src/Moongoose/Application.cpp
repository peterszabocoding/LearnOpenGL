#include "Application.h"
#include "Log.h"

#include "Moongoose/Events/ApplicationEvent.h"

namespace Moongoose
{
	Application::Application()
	{
		
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.is_in_category(EventCategory::Application))
		{
			LOG_APP_INFO(e.to_string());
		}

		while (true);
	}
}


