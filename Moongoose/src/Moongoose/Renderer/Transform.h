#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Moongoose {

	class Transform
	{
	public:
		Transform() {}
		Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale): position(pos), rotation(rot), scale(scale) {}
		~Transform() = default;

		glm::mat4 getModelMatrix() const;

	public:
		glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = glm::vec3{ 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = glm::vec3{ 1.0f, 1.0f, 1.0f };

	};

}
