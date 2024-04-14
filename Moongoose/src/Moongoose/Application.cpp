#include "mgpch.h"

#include <glad/glad.h>

#include "Log.h"
#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

namespace Moongoose
{

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		MG_ASSERT(!s_Instance, "Application has been initialized already");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->setEventCallback(BIND_EVENT_FUNC(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushLayer(m_ImGuiLayer);
	}

	Application::~Application() {}

	void Application::Run()
	{

		float time = glfwGetTime();
		float deltaTime = time - m_LastFrameTime;
		m_LastFrameTime = time;

		while (m_Running)
		{
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_LayerStack)
			{
				layer->onUpdate(deltaTime);
			}

			m_ImGuiLayer->begin();
			for (Layer* layer : m_LayerStack)
			{
				layer->onImGuiRender();
			}
			m_ImGuiLayer->end();

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

	bool Application::OnWindowClosed(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}
}


