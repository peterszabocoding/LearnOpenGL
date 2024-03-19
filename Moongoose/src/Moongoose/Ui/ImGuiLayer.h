#pragma once

#include "Moongoose/GuiLayer.h"

namespace Moongoose {
	
	class MOONGOOSE_API ImGuiLayer: public GuiLayer
	{

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onImGuiRender() override;
		virtual void Begin() override;
		virtual void End() override;

	private:
		void setStyle();
	};

}


