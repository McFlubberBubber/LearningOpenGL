#pragma once
#ifndef USERINPUT_H
#define USERINPUT_H

#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

enum class RenderMode {
	NORMAL,
	INVERT,
	GRAYSCALE,
	SHARPEN,
	DARK_SHARPEN,
	COUNT		// Extra enum type
};

enum class CameraMode {
	FREEFLY,
	FPS,
	COUNT		// Extra enum type
};

struct InputState {
	std::unordered_map<int, bool> last_key_states;
	CameraMode camera_mode { CameraMode::FREEFLY };
	RenderMode render_mode { RenderMode::NORMAL };

	bool is_key_pressed (GLFWwindow* window, int key) {
		bool current_key 	 = glfwGetKey(window, key) == GLFW_PRESS;
		bool key_was_pressed = last_key_states[key];
		last_key_states[key] = current_key;
		return current_key && !key_was_pressed;
	}

	bool is_key_released (GLFWwindow* window, int key) {
		bool current_key 	 = glfwGetKey(window, key) == GLFW_PRESS;
		bool key_was_pressed = last_key_states[key];
		last_key_states[key] = current_key;
		return !current_key && key_was_pressed;		
	}
};


// Function to handle user input
void processInput(GLFWwindow* window, Camera& camera, float deltaTime, InputState& input_state);


// Utility functions to help cycle through the enums
void switch_camera_mode(InputState& input);

static void apply_render_mode(RenderMode render_mode);
void increment_render_mode(InputState& input);
void decrement_render_mode(InputState& input);

// Functions that will process each camera mode
void do_fps_movement(GLFWwindow* window, Camera& camera, float deltaTime);
void do_freefly_movement(GLFWwindow* window, Camera& camera, float deltaTime);

// Function to store RenderMode state for future printing requirements
const char* render_mode_to_string(RenderMode render_mode);
#endif
