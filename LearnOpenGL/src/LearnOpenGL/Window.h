#pragma once

#include "pch.h"

#include "LearnOpenGL/Core.h"
#include "LearnOpenGL/Events/Event.h"

namespace LearnOpenGL {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(
			const std::string& title = "LearnOpenGL Engine",
			unsigned int width = 1920,
			unsigned int height = 1080) : Title(title), Width(width), Height(height)
		{}
	};

	class LEARNOPENGL_API Window
	{
	public:
		using EventCallback = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;

		virtual void setEventCallback(const EventCallback& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSyncEnabled() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};

}

