#pragma once

#include "Moongoose/Layer.h"

namespace Moongoose {
	
	class MOONGOOSE_API ImGuiLayer: public Layer
	{

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		void onUpdate() override;
		void onEvent(Event& event) override;

	protected:
		virtual bool onWindowResized(WindowResizeEvent& event) override;

		virtual bool onMouseButtonPresed(MousePressedEvent& event) override;
		virtual bool onMouseButtonReleased(MouseReleasedEvent& event) override;

		virtual bool onMouseMoved(MouseMovedEvent& event) override;
		virtual bool onMouseScrolled(MouseScrolledEvent& event) override;

		virtual bool onKeyPressed(KeyPressedEvent& event) override;
		virtual bool onKeyReleased(KeyReleasedEvent& event) override;
		virtual bool onKeyTyped(KeyTypedEvent& event) override;

	private:
		void setStyle();

	private:
		float m_Time = 0.0f;
	};

}


