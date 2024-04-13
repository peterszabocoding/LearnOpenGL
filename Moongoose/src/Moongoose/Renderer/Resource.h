#pragma once

#include <string>
#include "Moongoose/Core.h"
#include "Moongoose/Asset/UUID.h"

namespace Moongoose {

	enum class ResourceType : uint32_t {
		MODEL = 0,
		TEXTURE,
		MATERIAL,
		SCENE,
	};

	class Resource {

	public:
		Resource(UUID uuid, std::string resourceName, ResourceType resourceType) : m_UUID(uuid), m_ResourceName(resourceName), m_Type(resourceType) {};
		Resource(std::string resourceName, ResourceType resourceType) : m_UUID(UUID()), m_ResourceName(resourceName), m_Type(resourceType) {};
		~Resource() = default;

		const UUID GetUUID() const { return m_UUID; }
		std::string GetResourceName() const { return m_ResourceName; }
		const ResourceType GetResourceType() const { return m_Type; }

	private:
		const UUID m_UUID;
		const std::string m_ResourceName;
		const ResourceType m_Type;
	};

}