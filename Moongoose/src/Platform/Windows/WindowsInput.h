#pragma once

#include "Moongoose/Input/Input.h"

namespace Moongoose {

	class WindowsInput : public Input 
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMousePressedImpl(int button) override;

		virtual std::pair<float, float> GetMousePosImpl() override;
		virtual float GetMousePosXImpl() override;
		virtual float GetMousePosYImpl() override;
	};

}