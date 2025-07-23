#pragma once

#include "LearnOpenGL/Layer.h"

namespace LearnOpenGL {
	
	class LEARNOPENGL_API ImGuiLayer: public Layer
	{

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImGuiRender() override;

		void begin();
		void end();

	private:
		void setStyle();
		void showDockSpace(bool* p_open);
	};

}


