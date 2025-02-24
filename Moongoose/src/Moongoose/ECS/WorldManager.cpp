#include "mgpch.h"
#include <nlohmann/json.hpp>

#include "WorldManager.h"
#include "Moongoose/Renderer/Transform.h"
#include "Moongoose/Asset/AssetManager.h"
#include "Moongoose/Util/FileSystem.h"

namespace Moongoose
{
	Ref<World> WorldManager::createWorld(const std::string& name)
	{
		if (m_LoadedWorld) CloseWorld(m_LoadedWorld);

		m_LoadedWorld = CreateRef<World>();
		m_LoadedWorld->Init();

		m_LoadedWorld->m_ID = UUID();
		m_LoadedWorld->m_Name = name;

		m_LoadedWorld->RegisterComponent<IDComponent>();
		m_LoadedWorld->RegisterComponent<TagComponent>();
		m_LoadedWorld->RegisterComponent<TransformComponent>();
		m_LoadedWorld->RegisterComponent<MeshComponent>();
		m_LoadedWorld->RegisterComponent<LightComponent>();
		m_LoadedWorld->RegisterComponent<BillboardComponent>();
		m_LoadedWorld->RegisterComponent<AtmosphericsComponent>();
		m_LoadedWorld->RegisterComponent<PostProcessingVolumeComponent>();

		return m_LoadedWorld;
	}

	Ref<World> WorldManager::loadWorld(const std::string& sceneFile)
	{
		std::string worldJsonString = FileSystem::ReadFile(sceneFile);
		auto worldJson = nlohmann::json::parse(worldJsonString);

		std::string worldName = worldJson["name"];
		Ref<World> world = CreateWorld(worldName);

		auto entities = worldJson["entities"];

		for (nlohmann::json& jsonEntity : entities)
		{
			auto& jTagComponent = jsonEntity["TagComponent"];
			if (jTagComponent.empty()) continue;

			Entity entity = world->CreateEntity(jTagComponent["tag"]);

			auto& jIdComponent = jsonEntity["IDComponent"];
			if (!jIdComponent.empty())
			{
				IDComponent cID = world->GetComponent<IDComponent>(entity);
				std::string id = jIdComponent["id"];
				cID.m_ID = UUID(id);
			}

			auto& jTransformComponent = jsonEntity["TransformComponent"];
			if (!jTransformComponent.empty())
			{
				TransformComponent& cTransform = world->GetComponent<TransformComponent>(entity);
				cTransform.m_Position = glm::vec3(jTransformComponent["position"]["x"],
				                                  jTransformComponent["position"]["y"],
				                                  jTransformComponent["position"]["z"]);
				cTransform.m_Rotation = glm::vec3(jTransformComponent["rotation"]["x"],
				                                  jTransformComponent["rotation"]["y"],
				                                  jTransformComponent["rotation"]["z"]);
				cTransform.m_Scale = glm::vec3(jTransformComponent["scale"]["x"], jTransformComponent["scale"]["y"],
				                               jTransformComponent["scale"]["z"]);
			}

			auto& jMeshComponent = jsonEntity["MeshComponent"];
			if (!jMeshComponent.empty())
			{
				MeshComponent cMesh;
				std::string assetId = jMeshComponent["asset"];
				cMesh.m_Mesh = AssetManager::Get().LoadAssetFromFileById<Mesh>(UUID(assetId));
				world->AddComponent<MeshComponent>(entity, cMesh);
			}

			auto& jLightComponent = jsonEntity["LightComponent"];
			if (!jLightComponent.empty())
			{
				LightComponent cLight;

				cLight.m_Color = glm::vec3(
					jLightComponent["color"]["r"], 
					jLightComponent["color"]["g"],
				    jLightComponent["color"]["b"]
				);

				cLight.m_Intensity = jLightComponent["intensity"];
				cLight.m_Type = (LightType)jLightComponent["type"];

				cLight.m_AttenuationRadius = jLightComponent.contains("attenuationRadius")
					                             ? jLightComponent["attenuationRadius"]
					                             : 10.0f;

				cLight.m_AttenuationAngle = jLightComponent.contains("attenuationAngle")
					                            ? jLightComponent["attenuationAngle"]
					                            : 0.75f;
				cLight.m_AmbientIntensity = jLightComponent.contains("ambientIntensity")
					                            ? jLightComponent["ambientIntensity"]
					                            : 0.15f;

				cLight.m_ShadowType = jLightComponent.contains("shadowType")
					                      ? jLightComponent["shadowType"]
					                      : ShadowType::NONE;
				cLight.m_ShadowMapResolution = jLightComponent.contains("shadowResolution")
					                               ? ShadowMapResolution(jLightComponent["shadowResolution"])
					                               : ShadowMapResolution::MEDIUM;

				world->AddComponent<LightComponent>(entity, cLight);
			}

			auto& jAtmosphericsComponent = jsonEntity["AtmosphericsComponent"];
			if (!jAtmosphericsComponent.empty())
			{
				AtmosphericsComponent cAtmospherics;

				cAtmospherics.time = jAtmosphericsComponent["time"];
				cAtmospherics.m_SunIntensity = jAtmosphericsComponent["sun_intensity"];
				cAtmospherics.m_SunAmbientIntensity = jAtmosphericsComponent["sun_ambientIntensity"];

				cAtmospherics.m_SunColor = glm::vec3(
					jAtmosphericsComponent["sun_color"]["r"],
					jAtmosphericsComponent["sun_color"]["g"],
					jAtmosphericsComponent["sun_color"]["b"]
				);
				cAtmospherics.m_SunDirection = glm::vec3(
					jAtmosphericsComponent["sun_direction"]["x"],
					jAtmosphericsComponent["sun_direction"]["y"],
					jAtmosphericsComponent["sun_direction"]["z"]
				);

				world->AddComponent<AtmosphericsComponent>(entity, cAtmospherics);
			}

			auto& jPostProcessingVolumeComponent = jsonEntity["PostProcessingVolumeComponent"];
			if (!jPostProcessingVolumeComponent.empty())
			{
				PostProcessingVolumeComponent cPostProcessingVolumeComponent;

				cPostProcessingVolumeComponent.SSR_maxDistance = jPostProcessingVolumeComponent["SSR_maxDistance"];
				cPostProcessingVolumeComponent.SSR_resolution = jPostProcessingVolumeComponent["SSR_resolution"];
				cPostProcessingVolumeComponent.SSR_thickness = jPostProcessingVolumeComponent["SSR_thickness"];
				cPostProcessingVolumeComponent.SSR_steps = jPostProcessingVolumeComponent["SSR_steps"];

				world->AddComponent<PostProcessingVolumeComponent>(entity, cPostProcessingVolumeComponent);
			}
		}

		return world;
	}

	void WorldManager::saveWorld(const std::string& sceneFile) const
	{
		if (!m_LoadedWorld) return;

		auto filepath = std::filesystem::path(sceneFile);
		auto filename = filepath.filename().string();
		auto fileDir = filepath.parent_path().string();

		nlohmann::json jsonWorld = nlohmann::json::object();
		nlohmann::json jsonEntityArray = nlohmann::json::array();

		jsonWorld["name"] = m_LoadedWorld->m_Name;
		for (Entity entity = 0; entity < m_LoadedWorld->GetEntityCount(); entity++)
		{
			nlohmann::json entityObj = nlohmann::json::object();

			// IDComponent
			if (m_LoadedWorld->HasComponent<IDComponent>(entity))
			{
				nlohmann::json jId = nlohmann::json::object();
				auto& cId = m_LoadedWorld->GetComponent<IDComponent>(entity);

				jId["id"] = std::to_string(cId.m_ID);
				entityObj["IDComponent"] = jId;
			}

			// TagComponent
			if (m_LoadedWorld->HasComponent<TagComponent>(entity))
			{
				nlohmann::json jTag = nlohmann::json::object();
				auto& cTag = m_LoadedWorld->GetComponent<TagComponent>(entity);

				jTag["tag"] = cTag.Tag;
				entityObj["TagComponent"] = jTag;
			}

			// TransformComponent
			if (m_LoadedWorld->HasComponent<TransformComponent>(entity))
			{
				nlohmann::json jTransform = nlohmann::json::object();
				auto& cTransform = m_LoadedWorld->GetComponent<TransformComponent>(entity);

				jTransform["position"] = {
					{"x", cTransform.m_Position.x}, {"y", cTransform.m_Position.y}, {"z", cTransform.m_Position.z}
				};
				jTransform["rotation"] = {
					{"x", cTransform.m_Rotation.x}, {"y", cTransform.m_Rotation.y}, {"z", cTransform.m_Rotation.z}
				};
				jTransform["scale"] = {
					{"x", cTransform.m_Scale.x}, {"y", cTransform.m_Scale.y}, {"z", cTransform.m_Scale.z}
				};
				entityObj["TransformComponent"] = jTransform;
			}

			// MeshComponent
			if (m_LoadedWorld->HasComponent<MeshComponent>(entity))
			{
				nlohmann::json jMesh = nlohmann::json::object();
				auto& cMesh = m_LoadedWorld->GetComponent<MeshComponent>(entity);
				auto& decl = AssetManager::Get().GetDeclById(cMesh.m_Mesh->m_Id);

				jMesh["asset"] = std::to_string(decl.id);
				jMesh["assetDecl"] = decl.declFilePath;
				entityObj["MeshComponent"] = jMesh;
			}

			// LightComponent
			if (m_LoadedWorld->HasComponent<LightComponent>(entity))
			{
				nlohmann::json jLight = nlohmann::json::object();
				auto& cLight = m_LoadedWorld->GetComponent<LightComponent>(entity);

				jLight["type"] = cLight.m_Type;
				jLight["color"] = {{"r", cLight.m_Color.x}, {"g", cLight.m_Color.y}, {"b", cLight.m_Color.z}};
				jLight["intensity"] = cLight.m_Intensity;
				jLight["attenuationRadius"] = cLight.m_AttenuationRadius;
				jLight["attenuationAngle"] = cLight.m_AttenuationAngle;
				jLight["ambientIntensity"] = cLight.m_AmbientIntensity;
				jLight["shadowType"] = cLight.m_ShadowType;
				jLight["shadowResolution"] = cLight.m_ShadowMapResolution;

				entityObj["LightComponent"] = jLight;
			}

			// AtmosphericsComponent
			if (m_LoadedWorld->HasComponent<AtmosphericsComponent>(entity))
			{
				nlohmann::json jAtmospherics = nlohmann::json::object();
				auto& cAtmospherics = m_LoadedWorld->GetComponent<AtmosphericsComponent>(entity);

				jAtmospherics["time"] = cAtmospherics.time;
				jAtmospherics["sun_direction"] = { 
					{"x", cAtmospherics.m_SunDirection.x}, 
					{"y", cAtmospherics.m_SunDirection.y}, 
					{"z", cAtmospherics.m_SunDirection.z}
				};
				jAtmospherics["sun_color"] = {
					{"r", cAtmospherics.m_SunColor.r},
					{"g", cAtmospherics.m_SunColor.g},
					{"b", cAtmospherics.m_SunColor.b}
				};
				jAtmospherics["sun_intensity"] = cAtmospherics.m_SunIntensity;
				jAtmospherics["sun_ambientIntensity"] = cAtmospherics.m_SunAmbientIntensity;

				entityObj["AtmosphericsComponent"] = jAtmospherics;
			}


			// PostProcessingVolumeComponent
			if (m_LoadedWorld->HasComponent<PostProcessingVolumeComponent>(entity))
			{
				nlohmann::json jPostProcessing = nlohmann::json::object();
				auto& cPostProcessing = m_LoadedWorld->GetComponent<PostProcessingVolumeComponent>(entity);

				jPostProcessing["SSR_maxDistance"] = cPostProcessing.SSR_maxDistance;
				jPostProcessing["SSR_resolution"] = cPostProcessing.SSR_resolution;
				jPostProcessing["SSR_steps"] = cPostProcessing.SSR_steps;
				jPostProcessing["SSR_thickness"] = cPostProcessing.SSR_thickness;

				entityObj["PostProcessingVolumeComponent"] = jPostProcessing;
			}

			jsonEntityArray.push_back(entityObj);
		}

		jsonWorld["entities"] = jsonEntityArray;

		auto& outputFilename = fileDir + "\\" + m_LoadedWorld->m_Name + ".mgworld";
		if (!FileSystem::IsFileExist(fileDir)) FileSystem::MakeDirectory(fileDir);

		std::ofstream o(outputFilename);
		o << std::setw(4) << jsonWorld << std::endl;

		LOG_APP_INFO("Save World");
	}

	void WorldManager::closeWorld(Ref<World> world)
	{
		world.reset();
	}
}
