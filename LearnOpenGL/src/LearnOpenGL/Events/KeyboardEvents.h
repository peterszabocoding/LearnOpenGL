#pragma once

#include "LearnOpenGL/Events/Event.h"

namespace LearnOpenGL {

	class LEARNOPENGL_API KeyEvent : public Event
	{
	public:
		inline int getKeyCode() const { return m_Keycode; }

		EVENT_CLASS_CATEGORY(EventCategory::EventKeyboard | EventCategory::EventInput)

	protected:
		KeyEvent(int kcode): m_Keycode(kcode) {}

		int m_Keycode;
	};

	class LEARNOPENGL_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const
		{
			return m_RepeatCount;
		}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "Key pressed event: " << m_Keycode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int m_RepeatCount;
	};

	class LEARNOPENGL_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "Key released event: " << m_Keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)

	};

	class LEARNOPENGL_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "Key typed event: " << m_Keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)

	};
}