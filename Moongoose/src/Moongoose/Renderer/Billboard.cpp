#include "mgpch.h"
#include "Billboard.h"

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

Billboard::Billboard(glm::vec3 position)
{
	this->position = position;
	this->modelTransform = glm::mat4(1.0f);
}

void Billboard::Update(glm::vec3 cameraPosition)
{
	glm::vec3 direction = cameraPosition - position;
	float theta = glm::atan(direction.y, direction.x);
	float distance2D = glm::sqrt(direction.x * direction.x + direction.y * direction.y);
	float phi = glm::atan(direction.z, distance2D);

	modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, position);
	modelTransform = modelTransform * glm::eulerAngleXYZ(0.0f, phi, theta);
}
