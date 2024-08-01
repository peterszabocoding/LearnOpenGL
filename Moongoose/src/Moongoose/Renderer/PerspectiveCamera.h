#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Moongoose/Events/ApplicationEvent.h"
#include "Moongoose/Events/KeyboardEvents.h"
#include "Moongoose/Events/MouseEvents.h"
#include "Moongoose/Core.h"
#include "Camera.h"

namespace Moongoose {

	class PerspectiveCamera: public Camera
	{
	public:

		struct Params {
			unsigned int renderWidth = 1280;
			unsigned int renderHeight = 720;
			float fov = 45.0f;
			float zNear = 0.1f;
			float zFar = 1000.0f;
			float maxSpeed = 2.0f;

			glm::vec3 startPosition = glm::vec3{ 0.0f , 0.0f , 0.0f };
			glm::vec3 startUp = glm::vec3{ 0.0f , 1.0f , 0.0f };

			float startYaw = -90.0f;
			float startPitch = 0.0f;
			float startMoveSpeed = 0.5f;
			float startTurnSpeed = 0.3f;
		};

	public:
		PerspectiveCamera(const PerspectiveCamera::Params& params);
		~PerspectiveCamera() = default;

		void onUpdate(float deltaTime);
		void onEvent(Event& e);

		void setRenderResolution(float width, float height);

		void moveCamera(float deltaTime);
		void rotateCamera(float deltaTime);

		float getFOV() const;
		float getFOVRad() const;
		float getNear() const;
		float getFar() const;
		glm::vec3 getForward() const;
		glm::vec3 getCameraPosition() const;
		glm::mat4 getViewMatrix() const;


	private:
		bool onResize(WindowResizeEvent& event);
		bool onMouseScrolled(MouseScrolledEvent& event);

	private:
		bool m_IsCameraMoving = false;
		glm::vec2 mouseDelta = { 0.0f, 0.0f };
		glm::vec2 lastMousePos = { 0.0f, 0.0f };

		glm::vec3 m_Velocity;
		glm::vec3 m_Position;
		glm::vec3 m_Rotation = {0.0f, -90.0f, 0.0f}; // Pitch - Yaw - Roll

		glm::vec3 m_Front;
		glm::vec3 m_Up;
		glm::vec3 m_Right;
		glm::vec3 m_WorldUp;

		PerspectiveCamera::Params m_Params;

		float movementSpeed;
		float turnSpeed;

		float moveTransitionEffect = 0.5f;
		float maxSpeed = 0.1f;
		float cameraDrag = 0.975f;

	};

}
