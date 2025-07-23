#pragma once
#include <LearnOpenGL.h>
#include <imgui/imgui.h>

class ImGuiConfigLayer : public LearnOpenGL::Layer
{
public:
	ImGuiConfigLayer() 
	{
		ImGuiIO& io = ImGui::GetIO();

		const float fontSize = 16.5f;// *2.0f;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("fonts/opensans/OpenSans-Regular.ttf", fontSize);
		io.Fonts->AddFontFromFileTTF("fonts/opensans/OpenSans-ExtraBold.ttf", fontSize);
	}

	~ImGuiConfigLayer() override {}

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(LearnOpenGL::Event& event) override {}
	virtual void onImGuiRender() override {}
};



