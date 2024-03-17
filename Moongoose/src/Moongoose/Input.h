#pragma once

#include "Moongoose/Core.h"

namespace Moongoose {

	class MOONGOOSE_API Input
	{
	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		inline static bool IsMousePressed(int button) { return s_Instance->IsMousePressedImpl(button); }

		inline static float GetMousePosX() { return s_Instance->GetMousePosXImpl(); }
		inline static float GetMousePosY() { return s_Instance->GetMousePosYImpl(); }
		inline static std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMousePressedImpl(int button) = 0;

		virtual float GetMousePosXImpl() = 0;
		virtual float GetMousePosYImpl() = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;

	private:
		static Input* s_Instance;
	};

}