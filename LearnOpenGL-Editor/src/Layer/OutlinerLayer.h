#pragma once
#include <LearnOpenGL.h>

class OutlinerLayer : public LearnOpenGL::Layer
{
public:
	OutlinerLayer() = default;
	~OutlinerLayer() override = default;

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(LearnOpenGL::Event& event) override {}
	virtual void onImGuiRender() override;

private:
	static unsigned int m_NewEntityCounter;
};
