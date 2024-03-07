#pragma once

#include "Moongoose/Core.h"

namespace Moongoose {

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased,
		MousePressed, MouseReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		Application = BIT(0),
		Keyboard = BIT(1),
		Mouse = BIT(2)
	};

#define EVENT_CLASS_TYPE(type) static EventType get_static_type() { return EventType::##type; }\
								virtual EventType get_event_type() const override { return get_static_type(); }\
								virtual const char* get_name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int get_category_flags() const override { return category; }

	class MOONGOOSE_API Event
	{
	public:
		virtual EventType get_event_type() const = 0;
		virtual const char* get_name() const = 0;
		virtual int get_category_flags() const = 0;
		virtual std::string to_string() const { return get_name(); }

		inline bool is_in_category(EventCategory category)
		{
			return get_category_flags() & category;
		}

	protected:
		bool is_handled = false;
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
			if (event.get_event_type() != T::get_static_type()) return false;

			event.is_handled = callback(*(T*) &event);
			return true;
		}

	private:
		Event& event;
	};

}