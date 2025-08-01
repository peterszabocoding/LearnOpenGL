#include "pch.h"

/* Make sure to include glad.h before glfw3!*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "LearnOpenGL/Log.h"
#include "LearnOpenGL/Events/MouseEvents.h"
#include "LearnOpenGL/Events/KeyboardEvents.h"
#include "LearnOpenGL/Events/ApplicationEvent.h"
#include "Platform/Windows/WindowsWindow.h"

namespace LearnOpenGL {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LOG_CORE_ERROR("GLFW Error: {0} | {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	LearnOpenGL::WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		init(props);
	}

	LearnOpenGL::WindowsWindow::~WindowsWindow()
	{
		shutdown();
	}

	void LearnOpenGL::WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void LearnOpenGL::WindowsWindow::setVSync(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);
		m_Data.VSync = enabled;
	}

	bool LearnOpenGL::WindowsWindow::isVSyncEnabled() const
	{
		return m_Data.VSync;
	}

	void LearnOpenGL::WindowsWindow::init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		LOG_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			MG_CORE_ASSERT(success, "Could not initialize GLFW");

			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_SAMPLES, 4);
		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		MG_CORE_ASSERT(status, "Failed to initialize GLAD");

		glfwSetWindowUserPointer(m_Window, &m_Data);
		setVSync(false);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int w, int h)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = w;
				data.Height = h;

				data.EventCallback(WindowResizeEvent(w, h));
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.EventCallback(WindowCloseEvent());
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action) 
				{
				case GLFW_PRESS:
				{
					data.EventCallback(KeyPressedEvent(key, 0));
					break;
				}
				case GLFW_RELEASE:
				{
					data.EventCallback(KeyReleasedEvent(key));
					break;
				}
				case GLFW_REPEAT:
				{
					data.EventCallback(KeyPressedEvent(key, 1));
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int codepoint) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.EventCallback(KeyTypedEvent(codepoint));
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) 
			{
			case GLFW_PRESS:
			{
				data.EventCallback(MousePressedEvent(button));
				break;
			}
			case GLFW_RELEASE:
			{
				data.EventCallback(MouseReleasedEvent(button));
				break;
			}
			}

		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.EventCallback(MouseScrolledEvent((float) xOffset, (float) yOffset));
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.EventCallback(MouseMovedEvent((float)xPos, (float)yPos));
		});


		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_MULTISAMPLE);
	}

	void LearnOpenGL::WindowsWindow::shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

}
