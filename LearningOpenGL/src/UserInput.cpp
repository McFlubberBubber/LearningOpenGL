#include <iostream>
#include "UserInput.h"

// @TODO: So basically, this refactor has A LOT of if statement that
// are being used in this file which is most likely not the most
// optimized way of doing this. Therefore the user input system will
// need to be re-iterated if we want to further improve this mess.


static void apply_render_mode (RenderMode render_mode) {


}


void increment_render_mode (InputState& input_state) {
	int next_mode = (static_cast<int> (input_state.render_mode) + 1) % static_cast<int> (RenderMode::COUNT);

	input_state.render_mode = static_cast<RenderMode> (next_mode);
	apply_render_mode(input_state.render_mode);

	std::cout << "RenderMode::" << static_cast<int> (input_state.render_mode) << std::endl;
	
}

void decrement_render_mode(InputState& input_state) {
	int current_mode = static_cast<int> (input_state.render_mode);
	int total_modes  = static_cast<int> (RenderMode::COUNT);
	int prev_mode	 = (current_mode - 1 + total_modes) & total_modes;

	input_state.render_mode = static_cast<RenderMode> (prev_mode);
	apply_render_mode(input_state.render_mode);
	
	std::cout << "RenderMode::" << static_cast<int> (input_state.render_mode) << std::endl;

}



void switch_camera_mode(InputState& input_state) {
	// This line increments the enum by adding 1 and then using the
	// modular with the help of the extra enum type at the end to cycle
	// back to the start of the enums
	int next_mode = (static_cast<int> (input_state.camera_mode) + 1) % static_cast<int> (CameraMode::COUNT);

	input_state.camera_mode = static_cast<CameraMode> (next_mode);

	if(input_state.camera_mode == CameraMode::FPS) {
		std::cout << "FPS MODE ENABLED!" << std::endl;
	} else {
		std::cout << "FREE FLY MODE ENABLED!" << std::endl;
	}
}



// @TODO: Even though we refactored this entire user input section for
// better scaling (preparation for the post processing switches), these
// options will probably be moved to a menu later down the line for when
// we get text rendering out the way. But this should make creating
// keybindings a lot easier from now on.
void processInput(GLFWwindow* window, Camera& camera, float deltaTime, InputState& input_state) {
	
	// @NOTE This line is already being called in the render loop
	// glfwPollEvents();
	
	// Cycling through the camera mode enums	
	if (input_state.is_key_pressed(window, GLFW_KEY_E)) {
		switch_camera_mode(input_state);
	}

	// Either increment or decrement the different rendering modes
	if (input_state.is_key_pressed(window, GLFW_KEY_UP)) {
		increment_render_mode(input_state);
	}

	if (input_state.is_key_pressed(window, GLFW_KEY_DOWN)) {
		decrement_render_mode(input_state);
	}

	// Closing the window
	if (input_state.is_key_pressed(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}

	// Processing FPS movement
	if (input_state.camera_mode == CameraMode::FPS) {
		camera.position.y = -4.0f;
		do_fps_movement(window, camera, deltaTime);
	}

	// Processing free fly movement
	if (input_state.camera_mode == CameraMode::FREEFLY) {
		do_freefly_movement(window, camera, deltaTime);
	}
}


void do_fps_movement (GLFWwindow* window, Camera& camera, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processFPSMovement(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processFPSMovement(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processFPSMovement(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processFPSMovement(RIGHT, deltaTime);
}

void do_freefly_movement(GLFWwindow* window, Camera& camera, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processMovement(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processMovement(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processMovement(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processMovement(RIGHT, deltaTime);
}
