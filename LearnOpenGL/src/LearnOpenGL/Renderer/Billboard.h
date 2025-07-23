#pragma once

#include <glm/glm.hpp>
class Billboard
{
public:
	Billboard(glm::vec3 position);
	~Billboard() = default;

	void Update(glm::vec3 cameraPosition);

public:
		glm::vec3 position;
		glm::mat4 modelTransform;
};

