#include "mgpch.h"

#include "Moongoose/Log.h"
#include "WindowsWindow.h"

namespace Moongoose {

	static bool s_GLFWInitialized = false;

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	Moongoose::WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		init(props);
	}

	Moongoose::WindowsWindow::~WindowsWindow()
	{
		shutdown();
	}

	void Moongoose::WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void Moongoose::WindowsWindow::setVSync(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);
		m_Data.VSync = enabled;
	}

	bool Moongoose::WindowsWindow::isVSyncEnabled() const
	{
		return m_Data.VSync;
	}

	void Moongoose::WindowsWindow::init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		LOG_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			MG_CORE_ASSERT(success, "Could not initialize GLFW");

			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		setVSync(true);
	}

	void Moongoose::WindowsWindow::shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

}
