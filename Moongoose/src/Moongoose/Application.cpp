#include "mgpch.h"

#include <glad/glad.h>

#include "Log.h"
#include "GuiLayer.h"
#include "Application.h"
#include "Ui/ImGuiManager.h"

namespace Moongoose
{

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		MG_ASSERT(!s_Instance, "Application has been initialized already");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->setEventCallback(BIND_EVENT_FUNC(Application::OnEvent));

		m_ImGuiManager->startup();
	}

	Application::~Application()
	{
		m_ImGuiManager->shutdown();
	}

	void Application::Run()
	{
		bool show = true;
		while (m_Running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			//m_ImGuiManager->showDockSpace(&show);

			for (Layer* layer : m_LayerStack)
			{
				layer->onUpdate();
			}

			for (Layer* layer : m_GuiLayerStack)
			{
				static_cast<GuiLayer*>(layer)->Begin();
				static_cast<GuiLayer*>(layer)->onImGuiRender();
				static_cast<GuiLayer*>(layer)->End();
			}

			m_Window->OnUpdate();
		}
	}
	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClosed));
		
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->onEvent(event);
			if (event.isHandled()) break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushGuiLayer(Layer* layer)
	{
		m_GuiLayerStack.PushLayer(layer);
	}

	bool Application::OnWindowClosed(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}
}


