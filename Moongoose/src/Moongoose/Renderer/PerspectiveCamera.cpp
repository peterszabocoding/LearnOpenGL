#include "mgpch.h"

#include <GLFW/glfw3.h>
#include "PerspectiveCamera.h"
#include "Moongoose/Input/Input.h"
#include "Moongoose/Events/MouseEvents.h"
#include "Moongoose/Input/MouseButtonCodes.h"
#include "Moongoose/Input/KeyCodes.h"

namespace Moongoose {

	PerspectiveCamera::PerspectiveCamera(const PerspectiveCamera::Params& params)
	{
		m_Params = params;

		m_Position = params.startPosition;
		m_WorldUp = params.startUp;
		turnSpeed = params.startTurnSpeed;
		movementSpeed = params.startMoveSpeed;

		m_Front = glm::vec3(0.0f, 0.0f, -1.0f);

		m_Projection = glm::perspective(
			m_Params.fov,
			(float)m_Params.renderWidth / m_Params.renderHeight,
			m_Params.zNear,
			m_Params.zFar);

		onUpdate(1.0f);
	}

	void PerspectiveCamera::onUpdate(float deltaTime)
	{
		glm::vec2 mouseInput = {
			Input::GetMousePosX(),
			Input::GetMousePosY()
		};

		mouseDelta = lastMousePos - mouseInput;
		lastMousePos = mouseInput;


		if (Input::IsMousePressed(MG_MOUSE_BUTTON_RIGHT)) 
		{
			moveCamera(deltaTime);
			rotateCamera(deltaTime);
		}
		else 
		{
			moveTransitionEffect = 0.5f;
			m_Velocity = { 0.0f, 0.0f, 0.0f };
		}

		m_Front = glm::normalize(glm::vec3{
			 cos(glm::radians(m_Rotation.y))* cos(glm::radians(m_Rotation.x)),
			 sin(glm::radians(m_Rotation.x)),
			 sin(glm::radians(m_Rotation.y))* cos(glm::radians(m_Rotation.x))
		});

		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}

	void PerspectiveCamera::onEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(PerspectiveCamera::onResize));
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FUNC(PerspectiveCamera::onMouseScrolled));
	}

	bool PerspectiveCamera::onResize(WindowResizeEvent& event)
	{
		m_Params.renderWidth = event.getWidth();
		m_Params.renderHeight = event.getHeight();
		m_Projection = glm::perspective(
			m_Params.fov,
			(float) m_Params.renderWidth / m_Params.renderHeight, 
			m_Params.zNear,
			m_Params.zFar);

		return false;
	}

	bool PerspectiveCamera::onMouseScrolled(MouseScrolledEvent& event)
	{
		maxSpeed += event.GetYOffset() * 0.01;
		maxSpeed = std::clamp(maxSpeed, 0.05f, 10.0f);

		return false;
	}

	void PerspectiveCamera::moveCamera(float deltaTime)
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
			newVel = -m_Velocity;
		}
		else 
		{
			moveTransitionEffect += 0.75f * deltaTime;
			moveTransitionEffect = std::min(moveTransitionEffect, 1.0f);
			speed = moveTransitionEffect * movementSpeed * deltaTime;

			if (Input::IsKeyPressed(MG_KEY_W)) newVel += m_Front;
			if (Input::IsKeyPressed(MG_KEY_S)) newVel += -m_Front;
			if (Input::IsKeyPressed(MG_KEY_D)) newVel += m_Right;
			if (Input::IsKeyPressed(MG_KEY_A)) newVel += -m_Right;
			if (Input::IsKeyPressed(MG_KEY_SPACE)) newVel += m_WorldUp;
			if (Input::IsKeyPressed(MG_KEY_LEFT_CONTROL))	newVel += -m_WorldUp;
			if (Input::IsKeyPressed(MG_KEY_LEFT_SHIFT)) speed *= 2.0f;
		}
		speed = std::clamp(speed, 0.0f, maxSpeed);
		m_Velocity = cameraDrag * m_Velocity + 1.25f * (newVel * speed);
		m_Position += m_Velocity;
	}

	void PerspectiveCamera::rotateCamera(float deltaTime) {
		mouseDelta *= turnSpeed;

		m_Rotation.y -= mouseDelta.x;
		m_Rotation.x += mouseDelta.y;

		m_Rotation.x = std::clamp(m_Rotation.x, -89.0f, 89.0f);

		mouseDelta = { 0.0f, 0.0f };
	}

	glm::vec3 PerspectiveCamera::getCameraPosition() const
	{
		return m_Position;
	}

	glm::mat4 PerspectiveCamera::getViewMatrix() const
	{
		return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

}