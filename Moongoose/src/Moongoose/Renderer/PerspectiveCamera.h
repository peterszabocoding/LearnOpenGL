#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Moongoose/Events/ApplicationEvent.h"
#include "Moongoose/Events/KeyboardEvents.h"
#include "Moongoose/Events/MouseEvents.h"
#include "Moongoose/Core.h"

namespace Moongoose {

	class PerspectiveCamera
	{
	public:

		struct Params {
			GLuint renderWidth = 1280;
			GLuint renderHeight = 720;
			GLfloat fov = 45.0f;

			glm::vec3 startPosition = glm::vec3{ 0.0f , 0.0f , 0.0f };
			glm::vec3 startUp = glm::vec3{ 0.0f , 1.0f , 0.0f };

			GLfloat startYaw = -90.0f;
			GLfloat startPitch = 0.0f;
			GLfloat startMoveSpeed = 1.0f;
			GLfloat startTurnSpeed = 0.3f;

			float maxSpeed = 4.0f;
		};

	public:

		PerspectiveCamera(const PerspectiveCamera::Params& params);

		void init();
		void onUpdate(float deltaTime);
		void onEvent(Event& e);

		void keyControl();
		void mouseControl(GLfloat deltaX, GLfloat deltaY);

		glm::mat4& getProjection();
		glm::vec3 getCameraPosition() const;
		glm::mat4 calculateViewMatrix() const;

		~PerspectiveCamera();

	private:

		bool onResize(WindowResizeEvent& event);
		bool onMouseScrolled(MouseScrolledEvent& event);

	private:

		bool keys[1024];
		bool mouseButtons[2];
		bool m_IsCameraMoving = false;
		float mouseDeltaX, mouseDeltaY;

		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 worldUp;
		glm::vec3 velocity;

		GLuint renderWidth = 1280;
		GLuint renderHeight = 720;
		GLfloat fov = 45.0f;
		glm::mat4 projection;

		GLfloat yaw;
		GLfloat pitch;

		GLfloat movementSpeed;
		GLfloat turnSpeed;

		float deltaTime;
		float moveTransitionEffect = 0.0f;
		float maxSpeed = 0.1f;
		float cameraDrag = 0.9f;

		float lastMousePosX = 0.0f;
		float lastMousePosY = 0.0f;

	};

}
