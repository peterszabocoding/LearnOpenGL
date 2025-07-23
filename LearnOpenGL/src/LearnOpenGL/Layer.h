#pragma once

#include "LearnOpenGL/Core.h"
#include "LearnOpenGL/Events/Event.h"
#include "LearnOpenGL/Events/ApplicationEvent.h"
#include "LearnOpenGL/Events/MouseEvents.h"
#include "LearnOpenGL/Events/KeyboardEvents.h"

namespace LearnOpenGL {

class Application;

	class LEARNOPENGL_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() {};

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(float deltaTime) {}
		virtual void onEvent(Event& event) {}
		virtual void onImGuiRender() {};

		void SetApplication(Application* application) { m_Application = application; }

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		Application* GetApplication() 
		{ 
			return m_Application; 
		}

		virtual bool onWindowResized(WindowResizeEvent& event) { return false; }

		virtual bool onMouseButtonPressed(MousePressedEvent& event) { return false; }
		virtual bool onMouseButtonReleased(MouseReleasedEvent& event) { return false; }

		virtual bool onMouseMoved(MouseMovedEvent& event) { return false; }
		virtual bool onMouseScrolled(MouseScrolledEvent& event) { return false; }

		virtual bool onKeyPressed(KeyPressedEvent& event) { return false; }
		virtual bool onKeyReleased(KeyReleasedEvent& event) { return false; }
		virtual bool onKeyTyped(KeyTypedEvent& event) { return false; }

	protected:
		std::string m_DebugName;
		Application* m_Application;
	};

}