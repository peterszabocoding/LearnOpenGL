#include "pch.h"
#include "Transform.h"

namespace LearnOpenGL {

	glm::mat4 Transform::getModelMatrix() const
	{
		return glm::translate(glm::mat4(1.0f), position)
			* glm::toMat4(glm::quat(glm::radians(rotation))) 
			* glm::scale(glm::mat4(1.0f), scale);
	}

}


