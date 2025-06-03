#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//defining the different options for camera movement
enum camera_movement {
	FORWARD, BACKWARD, LEFT, RIGHT
};

//defining the default camera values
const float YAW			= -90.0f;
const float PITCH		= 0.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM		= 45.0f; 
const float SPEED		= 5.0f;
//const float SPRINTSPEED	= 1.0f;

//Camera class
class Camera {
public:
	//camera attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	//euler angles
	float yaw;
	float pitch;

	//camera options
	float movementSpeed;
	float sprintSpeed;
	float mouseSensitivity;
	float zoom;				//fov

	//constructor with vectors
	Camera
	(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW,
		float pitch = PITCH
	) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		m_updateCameraVectors();
	}

	//constructor with scalar values
	Camera
	(
		float posX, float posY, float posZ,
		float upX, float upY, float upZ,
		float yaw, float pitch
	) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		m_updateCameraVectors();
	}

	//returning the view matrix using eular angles and lookAt matrix
	glm::mat4 GetViewMatrix() 
	{
		return glm::lookAt(position, position + front, up);
	}

	//FREE FLY movement
	void processMovement(camera_movement direction, float deltaTime)
	{
		float velocity = movementSpeed * deltaTime;
		if (direction == FORWARD)
			position += front * velocity;
		if (direction == BACKWARD)
			position -= front * velocity;
		if (direction == LEFT)
			position -= right * velocity;
		if (direction == RIGHT)
			position += right * velocity;

		//logging XYZ coordinates
		std::cout << "X: " << position.x << " | Y: " << position.y << " | Z: " << position.z << std::endl;

	}

	//FPS movement
	void processFPSMovement(camera_movement direction, float deltaTime)
	{
		float velocity = movementSpeed * deltaTime;
		if (direction == FORWARD)
			position += front * velocity;
		if (direction == BACKWARD)
			position -= front * velocity;
		if (direction == LEFT)
			position -= right * velocity;
		if (direction == RIGHT)
			position += right * velocity;

		//locking to user to stay grounded
		position.y = 1.0f;

		//logging XYZ coordinates
		std::cout << "X: " << position.x << " | Y: " << position.y << " | Z: " << position.z << std::endl;
	}

	//processing mouse input 
	void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true)
	{
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		yaw += xOffset;
		pitch += yOffset;

		//making sure the pitch does not go out of bounds
		if (constrainPitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		//logging yaw and pitch values
		//std::cout << "YAW: " << yaw << " / PITCH: " << pitch << std::endl;
		//updating the camera vectors
		m_updateCameraVectors();
	}

	//processing the scroll for FOV changes
	void processMouseScroll(float yOffset) 
	{
		zoom -= (float)yOffset;
		if (zoom < 1.0f)
			zoom = 1.0f;
		if (zoom > 45.0f)
			zoom = 45.0f;
	}

private:
	void m_updateCameraVectors()
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(direction);
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}
};

#endif