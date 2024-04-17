#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Moongoose/Renderer/Mesh.h"
#include "Moongoose/Renderer/Shader.h"
#include "Moongoose/Renderer/Material.h"

namespace Moongoose {

	enum class ComponentType
	{
		Transform = 0,
		Mesh,
		Camera
	};

	struct Component
	{
		bool m_Active = false;
	};

	struct TransformComponent: public Component
	{
		glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	};

	struct MeshComponent: public Component
	{
		Ref<Mesh> m_Mesh = nullptr;
		Ref<Material> m_Material = nullptr;
		Ref<Shader> m_Shader = nullptr;
	};

}