#include <iostream>
#include "UserInput.h"
#include "Rendering.h"
#include "text_rendering.h"
#include "Time.h"


extern Font bold_text;

const float lifetime = 1.0f;
const float fade_duration = 1.0f;


bool InputState::is_key_pressed (GLFWwindow* window, int key) {
	bool current_key 	 = glfwGetKey(window, key) == GLFW_PRESS;
	bool key_was_pressed = last_key_states[key];
	last_key_states[key] = current_key;
	return current_key && !key_was_pressed;
}

bool InputState::is_key_released (GLFWwindow* window, int key) {
	bool current_key 	 = glfwGetKey(window, key) == GLFW_PRESS;
	bool key_was_pressed = last_key_states[key];
	last_key_states[key] = current_key;
	return !current_key && key_was_pressed;		
}


// @TODO: So basically, this refactor has A LOT of if statement that
// are being used in this file which is most likely not the most
// optimized way of doing this. Therefore the user input system will
// need to be re-iterated if we want to further improve this mess.

static void apply_render_mode (RenderMode render_mode) {
	const float x = RenderState::SCREEN_WIDTH / 2.0f;
	const float y = RenderState::SCREEN_HEIGHT / 1.5f;
	const float scale = 1.0f;
	const glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);
	const std::string tag = "render_mode";

	apply_render_mode_to_screen_shader(render_mode);
	bold_text.trigger_fading_text(tag, render_mode_to_string(render_mode), x, y, scale, color, lifetime, fade_duration, TextAlign::CENTER);

}


void increment_render_mode (InputState& input_state) {
	int next_mode = (static_cast<int> (input_state.render_mode) + 1) % static_cast<int> (RenderMode::COUNT);

	input_state.render_mode = static_cast<RenderMode> (next_mode);
	apply_render_mode(input_state.render_mode);
}


void decrement_render_mode(InputState& input_state) {
	int current_mode = static_cast<int> (input_state.render_mode);
	int total_modes  = static_cast<int> (RenderMode::COUNT);
	int prev_mode	 = (current_mode - 1 + total_modes) % total_modes;

	input_state.render_mode = static_cast<RenderMode> (prev_mode);
	apply_render_mode(input_state.render_mode);
}



void switch_camera_mode(InputState& input_state) {
	const float x = 100.0f;
	const float y = 100.0f;
	const float scale = 1.0f;
	const glm::vec3 color = glm::vec3(1.0f);
	const std::string tag = "camera_mode";
	const TextAlign align = TextAlign::LEFT;

	// This line increments the enum by adding 1 and then using the
	// modular with the help of the extra enum type at the end to cycle
	// back to the start of the enums
	int next_mode = (static_cast<int> (input_state.camera_mode) + 1) % static_cast<int> (CameraMode::COUNT);

	input_state.camera_mode = static_cast<CameraMode> (next_mode);

	if(input_state.camera_mode == CameraMode::FPS) {
		// std::cout << "FPS MODE ENABLED!" << std::endl;
		bold_text.trigger_fading_text(tag, "FPS Mode", x, y, scale, color, lifetime, fade_duration, align);
	} else {
		// std::cout << "FREE FLY MODE ENABLED!" << std::endl;
		bold_text.trigger_fading_text(tag, "Freefly Mode", x, y, scale, color, lifetime, fade_duration, align);
	}
}



// @TODO: Even though we refactored this entire user input section for
// better scaling (preparation for the post processing switches), these
// options will probably be moved to a menu later down the line for when
// we get text rendering out the way. But this should make creating
// keybindings a lot easier from now on.
void process_input(GLFWwindow* window, Camera& camera, InputState& input_state, ApplicationState& app_state) {
	float dt = Time::get_delta_time();

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

	// Processing either FPS or FREEFLY movement
	if (input_state.camera_mode == CameraMode::FPS) {
		camera.position.y = -4.0f;
		do_fps_movement(window, camera, dt);
	}
	if (input_state.camera_mode == CameraMode::FREEFLY) {
		do_freefly_movement(window, camera, dt);
	}

	// Toggling the application state
	if (input_state.is_key_pressed(window, GLFW_KEY_ESCAPE)) {
		if (app_state == ApplicationState::GAME) {
			app_state = ApplicationState::MENU;
		} else {
			app_state = ApplicationState::GAME;
	}			
	
	// Closing the window
/*
	if (input_state.is_key_pressed(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
*/
	}
}


void process_menu_navigation(GLFWwindow* window, InputState& input_state, ApplicationState& app_state, Menu& menu) {
	// Allowing the user to go back to the game without pressing "Resume"
	if (input_state.is_key_pressed(window, GLFW_KEY_ESCAPE))
		app_state = ApplicationState::GAME;


	// Cycling through MenuItem enum
	if (input_state.is_key_pressed(window, GLFW_KEY_UP))
		decrement_menu_item(menu);
	if (input_state.is_key_pressed(window, GLFW_KEY_DOWN))
		increment_menu_item(menu);


	// Checking for activations
	if (input_state.is_key_pressed(window, GLFW_KEY_ENTER)) {
		switch (menu.current_item) {
	
			case MenuItem::RESUME:
				app_state = ApplicationState::GAME;
				break;
		
			case MenuItem::MUSIC:
				menu.do_music = !menu.do_music;
				break;
		
			case MenuItem::QUIT:
				glfwSetWindowShouldClose(window, true);
				break;
		
			default:
				break;
		}
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

std::string render_mode_to_string(RenderMode render_mode) {
	switch (render_mode) {
		case RenderMode::NORMAL:			return "NORMAL";
		case RenderMode::INVERT:			return "INVERT";
		case RenderMode::GRAYSCALE:			return "GRAYSCALE";
		case RenderMode::SHARPEN:			return "SHARPEN";
		case RenderMode::DARK_SHARPEN:		return "DARK SHARPEN";
		default:							return "ERROR!";
	}
}
