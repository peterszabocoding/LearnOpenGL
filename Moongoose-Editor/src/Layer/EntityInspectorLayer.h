#pragma once
#include <Moongoose.h>

#include "imgui.h"

class ImVec2;

class EntityInspectorLayer : public Moongoose::Layer
{
public:
	EntityInspectorLayer() = default;
	~EntityInspectorLayer() override {}

	virtual void onAttach() override;
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(Moongoose::Event& event) override {}
	virtual void onImGuiRender() override;

private:
	static void RenderImageTextButton(ImVec2 imageSize, const Ref<Moongoose::Texture2D>& icon, const std::string& text);
	static void DrawMaterialControls(const Ref<Moongoose::Mesh>& mesh, unsigned int materialIndex);

	void DisplayAddMeshComponentEntry(const std::string& entryName, size_t entityId) const;
	void DisplayAddLightComponentEntry(const std::string& entryName, size_t entityId) const;
	void DisplayAddBillboardComponentEntry(const std::string& entryName, size_t entityId) const;
	void DisplayAddAtmosphericsComponentEntry(const std::string& entryName, size_t entityId) const;
	void DisplayAddPostProcessingComponentEntry(const std::string& entryName, size_t entityId) const;

private:
	Moongoose::AssetManager* m_AssetManager;
	Moongoose::WorldManager* m_WorldManager;
};
