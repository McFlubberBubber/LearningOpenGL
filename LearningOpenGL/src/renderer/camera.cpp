#include "camera.h"

// For full struct definitions
//#include "renderer/render_context.h"
#include "core/types.h"
#include "input/user_input.h"

Camera create_camera(glm::vec3 position, glm::vec3 world_up, float yaw, float pitch) {
	Camera camera = {};

	// Setting initial values
	camera.position = position;
	camera.world_up = world_up;
	camera.yaw		= yaw;
	camera.pitch	= pitch;

	// Setting default camera settings
	camera.movement_speed 	 = DEFAULT_SPEED;
	camera.sprint_speed		 = DEFAULT_SPEED * 2.0f;
	camera.mouse_sensitivity = DEFAULT_SENSITIVITY;
	camera.zoom				 = DEFAULT_ZOOM;

	// Initializing view matrix
	camera.view_matrix_dirty = true;
	update_camera_vectors(&camera);

	return camera;
}


glm::mat4 get_view_matrix(Camera* camera) {
	// Only recalculate if view matrix is dirty
	if (camera->view_matrix_dirty) {
		camera->view_matrix = glm::lookAt(
			camera->position,
			camera->position + camera->front,
			camera->up);
		camera->view_matrix_dirty = false;
	}

	return camera->view_matrix;
}


bool update_camera_from_input(GLFWwindow* window, CameraData* camera_data, InputState* input, float dt) {
	// Switching camera modes between freefly and fps.
	bool mode_changed = false;
	if (is_key_pressed(input, GLFW_KEY_E)) {
		switch_camera_mode(camera_data);
		camera_data->camera.position.y = 0.0f;	// Setting camera y pos to ground immediately.
		mode_changed = true;
	}
	
	Camera* camera = &camera_data->camera;
	
	// Movement keys - WASD
	if (is_key_down(input, GLFW_KEY_W)) {
		(camera_data->mode == CameraMode::FPS)
			? process_fps_movement(camera, CAMERA_FORWARD, dt)
			: process_camera_movement(camera, CAMERA_FORWARD, dt);
	}

	if (is_key_down(input, GLFW_KEY_S)) {
		(camera_data->mode == CameraMode::FPS)
			? process_fps_movement(camera, CAMERA_BACKWARD, dt)
			: process_camera_movement(camera, CAMERA_BACKWARD, dt);
	}

	if (is_key_down(input, GLFW_KEY_A)) {
		(camera_data->mode == CameraMode::FPS)
			? process_fps_movement(camera, CAMERA_LEFT, dt)
			: process_camera_movement(camera, CAMERA_LEFT, dt);
	}

	if (is_key_down(input, GLFW_KEY_D)) {
		(camera_data->mode == CameraMode::FPS)
			? process_fps_movement(camera, CAMERA_RIGHT, dt)
			: process_camera_movement(camera, CAMERA_RIGHT, dt);
	}

	// Handling sprint
	if (is_key_down(input, GLFW_KEY_LEFT_SHIFT))
		camera->movement_speed = camera->sprint_speed;
	else
		camera->movement_speed = DEFAULT_SPEED;

//	update_camera_vectors(camera);
	return mode_changed;
}


void switch_camera_mode(CameraData* camera_data) {
	s32 current_mode = static_cast<s32>(camera_data->mode);
	current_mode = (current_mode + 1) % static_cast<s32>(CameraMode::COUNT);
	camera_data->mode = static_cast<CameraMode>(current_mode);
}


void process_camera_movement(Camera* camera, CameraMovement direction, float dt) {
	float velocity = camera->movement_speed * dt;

	switch (direction) {
	case CAMERA_FORWARD:
		camera->position += camera->front * velocity;
		break;
	case CAMERA_BACKWARD:
		camera->position -= camera->front * velocity;
		break;
	case CAMERA_LEFT:
		camera->position -= camera->right * velocity;
		break;
	case CAMERA_RIGHT:
		camera->position += camera->right * velocity;
		break;
	}
	camera->view_matrix_dirty = true;
}


void process_fps_movement(Camera* camera, CameraMovement direction, float dt) {
	float velocity = camera->movement_speed * dt;

	// FPS Movement
	camera->position.y = 0.0f;
	glm::vec3 forward = glm::normalize(glm::vec3(camera->front.x, 0.0f, camera->front.z));
	glm::vec3 right = glm::normalize(glm::cross(forward, camera->world_up));

	switch (direction) {
	case CAMERA_FORWARD:
		camera->position += forward * velocity;
		break;
	case CAMERA_BACKWARD:
		camera->position -= forward * velocity;
		break;
	case CAMERA_LEFT:
		camera->position -= right * velocity;
		break;
	case CAMERA_RIGHT:
		camera->position += right * velocity;
		break;
	}
	camera->view_matrix_dirty = true;
}


void process_mouse_movement(Camera* camera, float x_offset, float y_offset, bool constrain_pitch) {
	x_offset *= camera->mouse_sensitivity;
	y_offset *= camera->mouse_sensitivity;

	camera->yaw += x_offset;
	camera->pitch += y_offset;

	if (constrain_pitch) {
		if (camera->pitch > 89.0f)
			camera->pitch = 89.0f;
		if (camera->pitch < -89.0f)
			camera->pitch = -89.0f;
	}

	// Updating camera vectors
	update_camera_vectors(camera);
	camera->view_matrix_dirty = true;
}


void process_mouse_scroll(Camera* camera, float y_offset) {
	camera->zoom -= y_offset;

	// Constraining zoom levels
	if (camera->zoom < 5.0f)
		camera->zoom = 5.0f;
	if (camera->zoom > 90.0f)
		camera->zoom = 90.0f;
}


void update_camera_vectors(Camera* camera) {
	// Calculate new front vector
	glm::vec3 front;
	front.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
	front.y = sin(glm::radians(camera->pitch));
	front.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));

	camera->front = glm::normalize(front);

	// Recalculate right and up vectors
	camera->right = glm::normalize(glm::cross(camera->front, camera->world_up));
	camera->up = glm::normalize(glm::cross(camera->right, camera->front));
}
