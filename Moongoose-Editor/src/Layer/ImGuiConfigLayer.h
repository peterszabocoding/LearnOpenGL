#pragma once
#include <Moongoose.h>
#include <imgui/imgui.h>

class ImGuiConfigLayer : public Moongoose::Layer
{
public:
	ImGuiConfigLayer() 
	{
		ImGuiIO& io = ImGui::GetIO();
		float fontSize = 18.0f;// *2.0f;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("fonts/opensans/OpenSans-Regular.ttf", fontSize);
		io.Fonts->AddFontFromFileTTF("fonts/opensans/OpenSans-ExtraBold.ttf", fontSize);
	}
	virtual ~ImGuiConfigLayer() {}

	virtual void onAttach() override {};
	virtual void onDetach() override {};
	virtual void onUpdate(float deltaTime) override {};
	virtual void onEvent(Moongoose::Event& event) override {};
	virtual void onImGuiRender() override {};
};



