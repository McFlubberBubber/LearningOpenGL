#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
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
	Camera (
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW,
		float pitch = PITCH
	);

	glm::mat4 GetViewMatrix() const;
	void processMovement(camera_movement direction, float deltaTime);
	void processFPSMovement(camera_movement direction, float deltaTime);
	void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);
	void processMouseScroll(float yOffset);

private:
	void m_updateCameraVectors();
};

#endif
