#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Forward declarations
struct InputState;
struct CameraData;

const float DEFAULT_YAW 			= -90.0f;
const float DEFAULT_PITCH 			= 0.0f;
const float DEFAULT_SENSITIVITY 	= 0.1f;
const float DEFAULT_ZOOM			= 75.0f;
const float DEFAULT_SPEED			= 5.0f;


enum class CameraMode {
	FREEFLY,
	FPS,
	COUNT		// Extra enum type
};

struct Camera {
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;

	float yaw;
	float pitch;

	float movement_speed;
	float sprint_speed;
	float mouse_sensitivity;
	float zoom; 	// FOV

	// Cached matrices (update when needed)
	glm::mat4 view_matrix;
	bool view_matrix_dirty;
};

enum CameraMovement {
	CAMERA_FORWARD,
	CAMERA_BACKWARD,
	CAMERA_LEFT,
	CAMERA_RIGHT
};

struct CameraData {
	Camera camera;
	CameraMode mode{ CameraMode::FREEFLY };		// Default
	glm::mat4 projection_matrix;
	float aspect_ratio;
	float near_plane;
	float far_plane;
};

Camera create_camera(
	glm::vec3 position = glm::vec3(0.0f),
	glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f),
	
	float yaw 	= DEFAULT_YAW,
	float pitch = DEFAULT_PITCH
	);


// Camera functionalities
glm::mat4 get_view_matrix(Camera* camera);

// Taking input from the user input system
bool update_camera_from_input(GLFWwindow* window, CameraData* camera_data, InputState* input_state, float dt);
void switch_camera_mode(CameraData* camera_data);

void process_camera_movement(Camera* camera, CameraMovement direction, float dt);
void process_fps_movement(Camera* camera, CameraMovement direction, float dt);
void process_mouse_movement(Camera* camera, float x_offset, float y_offset, bool constrain_pitch = true);
void process_mouse_scroll(Camera* camera, float y_offset);

// Internal helper
void update_camera_vectors(Camera* camera);
