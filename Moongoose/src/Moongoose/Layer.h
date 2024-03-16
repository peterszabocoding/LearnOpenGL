#pragma once

#include "Moongoose/Core.h"
#include "Moongoose/Events/Event.h"
#include "Moongoose/Events/ApplicationEvent.h"
#include "Moongoose/Events/MouseEvents.h"
#include "Moongoose/Events/KeyboardEvents.h"

namespace Moongoose {

	class MOONGOOSE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() {};

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate() {}
		virtual void onEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		virtual bool onWindowResized(WindowResizeEvent& event) { return false; }

		virtual bool onMouseButtonPresed(MousePressedEvent& event) { return false; }
		virtual bool onMouseButtonReleased(MouseReleasedEvent& event) { return false; }

		virtual bool onMouseMoved(MouseMovedEvent& event) { return false; }
		virtual bool onMouseScrolled(MouseScrolledEvent& event) { return false; }

		virtual bool onKeyPressed(KeyPressedEvent& event) { return false; }
		virtual bool onKeyReleased(KeyReleasedEvent& event) { return false; }
		virtual bool onKeyTyped(KeyTypedEvent& event) { return false; }

	protected:
		std::string m_DebugName;
	};

}