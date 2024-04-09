#include "mgpch.h"

#include <GLFW/glfw3.h>
#include "PerspectiveCamera.h"
#include "Moongoose/Input/Input.h"
#include "Moongoose/Events/MouseEvents.h"
#include "Moongoose/Input/MouseButtonCodes.h"
#include "Moongoose/Input/KeyCodes.h"

namespace Moongoose {

	PerspectiveCamera::PerspectiveCamera(const PerspectiveCamera::Params& params) :
		renderWidth(params.renderWidth), renderHeight(params.renderHeight), fov(params.fov),
		position(params.startPosition), worldUp(params.startUp),
		yaw(params.startYaw), pitch(params.startPitch),
		movementSpeed(params.startMoveSpeed), turnSpeed(params.startTurnSpeed)
	{
		front = glm::vec3(0.0f, 0.0f, -1.0f);

		mouseDeltaX = 0.0f;
		mouseDeltaY = 0.0f;

		for (unsigned int i = 0; i < 1024; i++) 
		{
			keys[i] = false;
		}

		for (unsigned int i = 0; i < 2; i++)
		{
			mouseButtons[i] = false;
		}

		init();

		projection = glm::perspective(fov, (GLfloat) renderWidth / (GLfloat) renderHeight, 0.1f, 1000.0f);
	}

	PerspectiveCamera::~PerspectiveCamera() {}

	void PerspectiveCamera::init()
	{
		onUpdate(1.0f);
	}

	void PerspectiveCamera::onUpdate(float deltaTime)
	{
		this->deltaTime = deltaTime;

		mouseDeltaX = lastMousePosX - Input::GetMousePosX();
		mouseDeltaY = lastMousePosY - Input::GetMousePosY();

		lastMousePosX = Input::GetMousePosX();
		lastMousePosY = Input::GetMousePosY();

		if (Input::IsMousePressed(MG_MOUSE_BUTTON_RIGHT)) 
		{
			keyControl();
			mouseControl(mouseDeltaX, mouseDeltaY);
		}
		else 
		{
			moveTransitionEffect = 0.5f;
			velocity = { 0.0f, 0.0f, 0.0f };
		}

		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		front = glm::normalize(front);
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}

	void PerspectiveCamera::onEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(PerspectiveCamera::onResize));
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FUNC(PerspectiveCamera::onMouseScrolled));
	}

	bool PerspectiveCamera::onResize(WindowResizeEvent& event)
	{
		renderWidth = event.getWidth();
		renderHeight = event.getHeight();
		projection = glm::perspective(fov, (GLfloat)renderWidth / (GLfloat)renderHeight, 0.1f, 1000.0f);

		return false;
	}

	bool PerspectiveCamera::onMouseScrolled(MouseScrolledEvent& event)
	{
		/*
		cameraDrag += event->yoffset * 0.01;
		cameraDrag = std::min(1.0f, cameraDrag);
		cameraDrag = std::max(0.1f, cameraDrag);
		LOG_CORE_INFO("Camera drag: {0}", cameraDrag);
		*/

		return false;
	}

	void PerspectiveCamera::keyControl()
	{
		m_IsCameraMoving = 
			Input::IsKeyPressed(MG_KEY_W) || 
			Input::IsKeyPressed(MG_KEY_S) ||
			Input::IsKeyPressed(MG_KEY_D) ||
			Input::IsKeyPressed(MG_KEY_A) ||
			Input::IsKeyPressed(MG_KEY_SPACE) ||
			Input::IsKeyPressed(MG_KEY_LEFT_CONTROL);
		
		float speed;
		glm::vec3 newVel = { 0.0f, 0.0f, 0.0f };

		if (!m_IsCameraMoving) {
			speed = 8.0f * maxSpeed * deltaTime;
			newVel = -velocity;
		}
		else {
			moveTransitionEffect += 0.75f * deltaTime;
			moveTransitionEffect = std::min(moveTransitionEffect, 1.0f);
			speed = moveTransitionEffect * movementSpeed * deltaTime;

			if (Input::IsKeyPressed(MG_KEY_LEFT_SHIFT)) {
				speed *= 2.0f;
			}

			if (Input::IsKeyPressed(MG_KEY_W)) newVel += front;
			if (Input::IsKeyPressed(MG_KEY_S)) newVel += -front;
			if (Input::IsKeyPressed(MG_KEY_D)) newVel += right;
			if (Input::IsKeyPressed(MG_KEY_A)) newVel += -right;
			if (Input::IsKeyPressed(MG_KEY_SPACE)) newVel += worldUp;
			if (Input::IsKeyPressed(MG_KEY_LEFT_CONTROL))	newVel += -worldUp;
		}
		speed = std::clamp(speed, 0.0f, maxSpeed);
		velocity = cameraDrag * velocity + 1.25f * (newVel * speed);
		position += velocity;
	}

	void PerspectiveCamera::mouseControl(GLfloat deltaX, GLfloat deltaY) {
		deltaX *= turnSpeed;
		deltaY *= turnSpeed;

		yaw += deltaX;
		pitch += deltaY;

		pitch = fmin(pitch, 89.0f);
		pitch = fmax(pitch, -89.0f);

		mouseDeltaX = 0.0f;
		mouseDeltaY = 0.0f;
	}

	glm::mat4& PerspectiveCamera::getProjection()
	{
		return projection;
	}

	glm::vec3 PerspectiveCamera::getCameraPosition() const
	{
		return position;
	}

	glm::mat4 PerspectiveCamera::calculateViewMatrix() const
	{
		return glm::lookAt(position, position + front, up);
	}

}