#pragma once
#include <Moongoose.h>
#include "Moongoose/ECS/WorldManager.h"

class ImVec2;

class EntityInspectorLayer : public Moongoose::Layer
{
public:
	EntityInspectorLayer() {};
	virtual ~EntityInspectorLayer() {};

	virtual void onAttach() override {};
	virtual void onDetach() override {};
	virtual void onUpdate(float deltaTime) override {};
	virtual void onEvent(Moongoose::Event& event) override {};
	virtual void onImGuiRender() override;

private:
	void DrawMaterialControls(Ref<Moongoose::Mesh> mesh, unsigned int materialIndex);

	void RenderImageTextButton(ImVec2 imageSize, Ref<Moongoose::Texture2D> icon, std::string text);

	template<typename T>
	void DisplayAddComponentEntry(const std::string& entryName, size_t entityId) {
		bool hasComponent = WorldManager::Get().GetLoadedWorld()->HasComponent<T>(entityId);

		if (!hasComponent)
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				ImGui::CloseCurrentPopup();
			}
		}
	}

	void DisplayAddMeshComponentEntry(const std::string& entryName, size_t entityId);
	void DisplayAddLightComponentEntry(const std::string& entryName, size_t entityId);
	void DisplayAddBillboardComponentEntry(const std::string& entryName, size_t entityId);
	void DisplayAddAtmosphericsComponentEntry(const std::string& entryName, size_t entityId);

};
