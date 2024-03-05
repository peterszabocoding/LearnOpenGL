#pragma once
#include "Core.h"

namespace Moongoose
{
	class MOONGOOSE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}

