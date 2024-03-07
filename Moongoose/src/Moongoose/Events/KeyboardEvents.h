#pragma once

#include "Moongoose/Events/Event.h"

namespace Moongoose {

	class MOONGOOSE_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return keycode; }

		EVENT_CLASS_CATEGORY(EventCategory::Keyboard)

	protected:
		KeyEvent(int kcode): keycode(kcode) {}

		int keycode;
	};

}