#pragma once

#include "Moongoose/Layer.h"


namespace Moongoose {

	class MOONGOOSE_API GuiLayer: public Layer
	{
	public:
		GuiLayer(const std::string& name = "Layer") : Layer(name) {}
		virtual ~GuiLayer() {};

		virtual void onImGuiRender() {}
		virtual void Begin() {};
		virtual void End() {};
	};

}