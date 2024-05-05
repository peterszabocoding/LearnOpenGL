#include "InspectorLayer.h"
#include <imgui/imgui.h>
#include "GuiWidgets.h"
#include "Moongoose/Renderer/Material.h"
#include "Moongoose/Renderer/OpenGL/OpenGLTexture2D.h"
#include "Platform/PlatformUtils.h"

using namespace Moongoose;

void InspectorLayer::onImGuiRender()
{
	ImGui::Begin("Inspector");

	auto selectedEntity = EntityManager::Get().getSelectedEntity();
	if (selectedEntity == -1) {
		ImGui::End();
		return;
	}

	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("Add Component");
	}

	if (ImGui::BeginPopup("Add Component"))
	{
		DisplayAddComponentEntry<LightComponent>("Light Component", selectedEntity);
		DisplayAddComponentEntry<MeshComponent>("Mesh Component", selectedEntity);

		ImGui::EndPopup();
	}

	bool hasTransformComponent = EntityManager::Get().hasComponent<TransformComponent>(EntityManager::Get().getEntities()[selectedEntity]);
	bool hasMeshComponent = EntityManager::Get().hasComponent<MeshComponent>(EntityManager::Get().getEntities()[selectedEntity]);
	bool hasLightComponent = EntityManager::Get().hasComponent<LightComponent>(EntityManager::Get().getEntities()[selectedEntity]);

	
	if (hasTransformComponent)
	{
		auto& tag = EntityMemoryPool::Get().getTag(selectedEntity);
		auto& cTransform = EntityManager::Get().getComponent<TransformComponent>(selectedEntity);

		auto windowSize = ImGui::GetWindowSize();

		ImGui::Dummy({ windowSize.x , 1.0f });

		static char tagArray[50];
		strcpy(tagArray, tag.c_str());
		ImGui::InputText("##label", tagArray, IM_ARRAYSIZE(tagArray));

		std::string newTag = std::string(tagArray);
		if (newTag != tag) EntityManager::Get().setTag(selectedEntity, std::string(tagArray));

		ImGui::Dummy({ windowSize.x , 5.0f });
		ImGui::Separator();

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Transform")) {
			GuiWidgets::DrawVec3Control("Position", cTransform.m_Position, 0.0f, windowSize.x / 4);
			GuiWidgets::DrawVec3Control("Rotation", cTransform.m_Rotation, 0.0f, windowSize.x / 4);
			GuiWidgets::DrawVec3Control("Scale", cTransform.m_Scale, 1.0f, windowSize.x / 4);
			ImGui::Dummy({ windowSize.x , 10.0f });

			ImGui::TreePop();
		}
	}

	if (hasMeshComponent)
	{
		auto& cMesh = EntityManager::Get().getComponent<MeshComponent>(selectedEntity);

		if (ImGui::TreeNode("Mesh Renderer")) {
			
			if (ImGui::TreeNode("Mesh"))
			{
				ImGui::Text("Mesh: ");
				ImGui::SameLine();

				if (cMesh.m_Mesh)
				{
					ImGui::Text(cMesh.m_Mesh->GetModelSource().c_str());
				}
				else
				{
					ImGui::Text("Empty");
				}
				ImGui::SameLine();

				if (ImGui::Button("Load"))
				{
					std::string& filePath = FileDialogs::OpenFile("Mesh Object (*.obj)\0*.obj\0");
					LOG_APP_INFO(filePath);
					if (!filePath.empty())
					{
						Ref<Moongoose::Material> material = CreateRef<Moongoose::Material>();
						Ref<Moongoose::Shader> shader = AssetManager::Get().LoadShader("Shader/shader.vert", "Shader/shader.frag");
						material->setShader(shader);
						material->setAlbedo(AssetManager::Get().LoadAsset<Texture2D>("Assets/Texture/checker_2k_c.png"));

						auto& relativePath = std::filesystem::relative(filePath, std::filesystem::current_path());
						cMesh.m_Mesh = AssetManager::Get().LoadAsset<Mesh>(relativePath.string());
						cMesh.m_Material = material;
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Materials")) {
				DrawMaterialControls(cMesh.m_Material);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}

	if (hasLightComponent)
	{
		auto& cLight = EntityManager::Get().getComponent<LightComponent>(selectedEntity);
		auto windowSize = ImGui::GetWindowSize();
		GuiWidgets::DrawFloatControl("Intensity", cLight.m_Intensity, 0.0f, 10000.0f, 0.1f, 1.0f, windowSize.x);
		GuiWidgets::DrawRGBColorPicker("Color", cLight.m_Color, 1.0f, windowSize.x);
	}

	ImGui::End();
}

void InspectorLayer::DrawMaterialControls(Ref<Moongoose::Material> material)
{
	if (!material) return;

	auto albedo = material->getAlbedo();
	ImGui::PushID("Material");

	ImGui::Text("Albedo Map: ");
	ImGui::SameLine();

	if (albedo) {
		GuiWidgets::DrawTextureImage(albedo->getPointerToData(), ImVec2{ 128.0f, 128.0f });
	}

	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		std::string& filePath = FileDialogs::OpenFile("PNG Image (*.png)\0*.png\0");
		if (!filePath.empty())
		{
			auto& relativePath = std::filesystem::relative(filePath, std::filesystem::current_path());
			material->setAlbedo(AssetManager::Get().LoadAsset<Texture2D>(relativePath.string()));
		}
	}

	ImGui::PopID();
	ImGui::Separator();
}
