#include "mgpch.h"
#include "Moongoose/Util/FileSystem.h"

#include "Moongoose/Asset/AssetManager.h"
#include "WorldManager.h"

#include <nlohmann/json.hpp>

#include "Moongoose/Renderer/Transform.h"
#include "Systems/BillboardSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/EntityListSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/AtmosphericsSystem.h"

namespace Moongoose {

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

		m_LoadedWorld->RegisterSystem<EntityListSystem>();
		m_LoadedWorld->RegisterSystem<LightSystem>();
		m_LoadedWorld->RegisterSystem<RenderSystem>();
		m_LoadedWorld->RegisterSystem<BillboardSystem>();
		m_LoadedWorld->RegisterSystem<AtmosphericsSystem>();

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
			auto jTagComponent = jsonEntity["TagComponent"];
			if (jTagComponent.empty()) continue;

			Entity entity = world->CreateEntity(jTagComponent["tag"]);

			auto jIdComponent = jsonEntity["IDComponent"];
			if (!jIdComponent.empty())
			{
				IDComponent cID = world->GetComponent<IDComponent>(entity);
				std::string id = jIdComponent["id"];
				cID.m_ID = UUID(id);
			}

			auto jTransformComponent = jsonEntity["TransformComponent"];
			if (!jTransformComponent.empty())
			{
				TransformComponent& cTransform = world->GetComponent<TransformComponent>(entity);
				cTransform.m_Position = glm::vec3(jTransformComponent["position"]["x"], jTransformComponent["position"]["y"], jTransformComponent["position"]["z"]);
				cTransform.m_Rotation = glm::vec3(jTransformComponent["rotation"]["x"], jTransformComponent["rotation"]["y"], jTransformComponent["rotation"]["z"]);
				cTransform.m_Scale = glm::vec3(jTransformComponent["scale"]["x"], jTransformComponent["scale"]["y"], jTransformComponent["scale"]["z"]);
			}

			auto jMeshComponent = jsonEntity["MeshComponent"];
			if (!jMeshComponent.empty())
			{
				MeshComponent cMesh;
				std::string assetId = jMeshComponent["asset"];
				cMesh.m_Mesh = AssetManager::Get().LoadAssetFromFileById<Mesh>(UUID(assetId));
				world->AddComponent<MeshComponent>(entity, cMesh);
			}

			auto jLightComponent = jsonEntity["LightComponent"];
			if (!jLightComponent.empty())
			{
				LightComponent cLight;
				cLight.m_Color = glm::vec3(jLightComponent["color"]["r"], jLightComponent["color"]["g"], jLightComponent["color"]["b"]);
				cLight.m_Intensity = jLightComponent["intensity"];
				cLight.m_Type = (LightType) jLightComponent["type"];
				cLight.m_AttenuationRadius = jLightComponent.contains("attenuationRadius") ? jLightComponent["attenuationRadius"] : 10.0f;
				cLight.m_AttenuationAngle = jLightComponent.contains("attenuationAngle") ? jLightComponent["attenuationAngle"] : 0.75f;
				cLight.m_AmbientIntensity = jLightComponent.contains("ambientIntensity") ? jLightComponent["ambientIntensity"] : 0.15f;
				cLight.m_ShadowType = jLightComponent.contains("shadowType") ? jLightComponent["shadowType"] : ShadowType::NONE;
				cLight.m_ShadowMapResolution = jLightComponent.contains("shadowResolution")
				? ShadowMapResolution(jLightComponent["shadowResolution"])
				: ShadowMapResolution::MEDIUM;

				world->AddComponent<LightComponent>(entity, cLight);
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

				jTransform["position"] = { {"x", cTransform.m_Position.x}, {"y", cTransform.m_Position.y}, {"z", cTransform.m_Position.z} };
				jTransform["rotation"] = { {"x", cTransform.m_Rotation.x}, {"y", cTransform.m_Rotation.y}, {"z", cTransform.m_Rotation.z} };
				jTransform["scale"] = { {"x", cTransform.m_Scale.x}, {"y", cTransform.m_Scale.y}, {"z", cTransform.m_Scale.z} };
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
				jLight["color"] = { {"r", cLight.m_Color.x}, {"g", cLight.m_Color.y}, {"b", cLight.m_Color.z} };
				jLight["intensity"] = cLight.m_Intensity;
				jLight["attenuationRadius"] = cLight.m_AttenuationRadius;
				jLight["attenuationAngle"] = cLight.m_AttenuationAngle;
				jLight["ambientIntensity"] = cLight.m_AmbientIntensity;
				jLight["shadowType"] = cLight.m_ShadowType;
				jLight["shadowResolution"] = cLight.m_ShadowMapResolution;

				entityObj["LightComponent"] = jLight;
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
