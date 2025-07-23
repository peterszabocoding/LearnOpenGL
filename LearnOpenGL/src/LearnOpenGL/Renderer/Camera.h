#pragma once

#include <glm/glm.hpp>

namespace LearnOpenGL {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection): m_Projection(projection) {}
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
		void SetCameraActive(const bool active) { isCameraActive = active; }

	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
		bool isCameraActive = false;
	};

}