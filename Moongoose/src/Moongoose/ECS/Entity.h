#pragma once

#include "Moongoose/Core.h"
#include "Components.h"
#include "Moongoose/Renderer/Transform.h"
#include <unordered_set>

namespace Moongoose {

	class Entity
	{
	public:
		Entity(std::string name): m_Name(name), m_ID(UUID()) {}
		~Entity() {}

	public:
		UUID m_ID;
		std::string m_Name;
		Ref<TransformComponent> m_TransformComponent;
		Ref<MeshComponent> m_MeshComponent;
	};

}