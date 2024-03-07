#pragma once

#include "Moongoose/Events/Event.h"

namespace Moongoose {

	class MOONGOOSE_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int w, unsigned int h)
			: width(w), height(h) {}

		inline unsigned int get_width() const { return width; }
		inline unsigned int get_height() const { return height; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << width << ", " << height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategory::Application)
	private:
		unsigned int width, height;
	};

	class MOONGOOSE_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategory::Application)
	};

}