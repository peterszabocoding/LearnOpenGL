#pragma once

#include "Moongoose/Window.h"

struct GLFWwindow;

namespace Moongoose {
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int getWidth() const override { return m_Data.Width; }
		inline unsigned int getHeight() const override { return m_Data.Height; }

		inline void setEventCallback(const EventCallback& callback) override { m_Data.EventCallback = callback; }
		void setVSync(bool enabled) override;
		bool isVSyncEnabled() const override;

		virtual void* GetNativeWindow() const override { return m_Window; }

	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();

	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			EventCallback EventCallback;
		};

		WindowData m_Data;

	};
}
