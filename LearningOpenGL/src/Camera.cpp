#include "Camera.h"

//constructor with vectors
Camera::Camera (
	glm::vec3 position,
	glm::vec3 up,
	float yaw,
	float pitch) 
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	this->position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
	m_updateCameraVectors();
}

//returning the view matrix using eular angles and lookAt matrix
glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(position, position + front, up);
}

//FREE FLY movement
void Camera::processMovement(camera_movement direction, float deltaTime)
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
void Camera::processFPSMovement(camera_movement direction, float deltaTime)
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
	position.y = -4.0f;

	//logging XYZ coordinates
	std::cout << "X: " << position.x << " | Y: " << position.y << " | Z: " << position.z << std::endl;
}

//processing mouse input 
void Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch)
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
void Camera:: processMouseScroll(float yOffset)
{
	zoom -= (float)yOffset;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}

void Camera::m_updateCameraVectors()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(direction);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}
