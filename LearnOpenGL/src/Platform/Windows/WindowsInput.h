#pragma once

#include "LearnOpenGL/Input/Input.h"

namespace LearnOpenGL {

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