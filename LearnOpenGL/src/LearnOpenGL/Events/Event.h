#pragma once

#include "pch.h"
#include "LearnOpenGL/Core.h"

namespace LearnOpenGL {

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased, KeyTyped,
		MousePressed, MouseReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventApplication = BIT(0),
		EventKeyboard = BIT(1),
		EventMouse = BIT(2),
		EventInput = BIT(3),
	};

#define BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::##type; }\
								virtual EventType getEventType() const override { return getStaticType(); }\
								virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

	class LEARNOPENGL_API Event
	{
	friend class EventDispatcher;
	public:

		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }
		
		inline bool isHandled() const { return handled; }

		inline bool isInCategory(EventCategory category)
		{
			return getCategoryFlags() & category;
		}

	protected:
		bool handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventCallback = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& e) : event(e) {}

		template<typename T>
		bool Dispatch(EventCallback<T> callback)
		{
			if (event.getEventType() != T::getStaticType()) return false;

			event.handled = callback(*(T*) &event);
			return true;
		}

	private:
		Event& event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.toString();
	}

}