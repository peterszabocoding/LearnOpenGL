#pragma once

#include "Moongoose/Events/Event.h"

namespace Moongoose {

	class MOONGOOSE_API KeyEvent : public Event
	{
	public:
		inline int getKeyCode() const { return keycode; }

		EVENT_CLASS_CATEGORY(EventCategory::EventKeyboard)

	protected:
		KeyEvent(int kcode): keycode(kcode) {}

		int keycode;
	};

}