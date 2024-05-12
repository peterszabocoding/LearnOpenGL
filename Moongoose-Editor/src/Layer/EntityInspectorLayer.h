#pragma once
#include <Moongoose.h>
#include "Moongoose/ECS/EntityManager.h"

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
	void DrawMaterialControls(Ref<Moongoose::Material> material);

	template<typename T>
	void DisplayAddComponentEntry(const std::string& entryName, size_t entityId) {
		bool hasComponent = EntityManager::Get().hasComponent<T>(entityId);

		if (!hasComponent)
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				T& component = EntityMemoryPool::Get().addComponent<T>(entityId);
				ImGui::CloseCurrentPopup();
			}
		}
	}

};
