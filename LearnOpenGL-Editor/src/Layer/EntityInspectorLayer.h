#pragma once
#include <LearnOpenGL.h>

#include "imgui.h"

class ImVec2;

class EntityInspectorLayer : public LearnOpenGL::Layer
{
public:
	EntityInspectorLayer() = default;
	~EntityInspectorLayer() override {}

	virtual void onAttach() override;
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(LearnOpenGL::Event& event) override {}
	virtual void onImGuiRender() override;

private:
	static void RenderImageTextButton(ImVec2 imageSize, const Ref<LearnOpenGL::Texture2D>& icon, const std::string& text);
	static void DrawMaterialControls(const Ref<LearnOpenGL::Mesh>& mesh, unsigned int materialIndex);

	void DisplayAddMeshComponentEntry(const std::string& entryName, size_t entityId) const;
	void DisplayAddLightComponentEntry(const std::string& entryName, size_t entityId) const;
	void DisplayAddBillboardComponentEntry(const std::string& entryName, size_t entityId) const;
	void DisplayAddAtmosphericsComponentEntry(const std::string& entryName, size_t entityId) const;
	void DisplayAddPostProcessingComponentEntry(const std::string& entryName, size_t entityId) const;

private:
	LearnOpenGL::AssetManager* m_AssetManager;
	LearnOpenGL::WorldManager* m_WorldManager;
};
