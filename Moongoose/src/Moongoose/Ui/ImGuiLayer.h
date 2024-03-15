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

	private:
		float m_Time = 0.0f;
	};

}


