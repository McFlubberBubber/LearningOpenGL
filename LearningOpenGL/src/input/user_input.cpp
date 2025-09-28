#include "user_input.h"

#include <iostream>

#include "renderer/render_context.h"
#include "renderer/render_system.h"
#include "core/program_state.h"
#include "core/time.h"
#include "ui/debug_overlay.h"
#include "ui/menu.h"


void update_input_state(InputState* input, GLFWwindow* window) {
	// Copy current state into the last
	input->last_key_states = input->key_states;

	// Updating current states
	for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++) {
		int state = glfwGetKey(window, key);
		input->key_states[key] = (state == GLFW_PRESS);
	}

//	input->scroll_delta = 0.0f;
}


void handle_menu_input(GLFWwindow* window, InputState* input, Menu* menu, ApplicationState& app) {
	// Allowing user to go back to game state
	if (is_key_pressed(input, GLFW_KEY_ESCAPE) && app == ApplicationState::MENU)
		app = ApplicationState::GAME;

	// Handling the increment + decrement of menu choices
	if (is_key_pressed(input, GLFW_KEY_UP))
		decrement_menu_item(menu);

	if (is_key_pressed(input, GLFW_KEY_DOWN))
		increment_menu_item(menu);


	// Checking for activations
	if (is_key_pressed(input, GLFW_KEY_ENTER)) {
		switch (menu->current_item) {
				case MenuItem::RESUME:
				app = ApplicationState::GAME;
				break;
		
			case MenuItem::MUSIC:
				menu->do_music = !menu->do_music;
				break;
		
			case MenuItem::QUIT:
				glfwSetWindowShouldClose(window, true);
				break;
		
			default:
				break;
		}
	}
}


void handle_game_input(GLFWwindow* window, InputState* input, RenderingContext* ctx, float dt, ApplicationState& app) {
	// Allowing the user to switch to menu
	if(is_key_pressed(input, GLFW_KEY_ESCAPE) && app == ApplicationState::GAME)
		app = ApplicationState::MENU;
	
    // Handle first mouse movement to prevent camera jump
    if (input->first_mouse) {
        input->last_mouse_x = input->mouse_x;
        input->last_mouse_y = input->mouse_y;
        input->first_mouse = false;
        // Skip mouse processing this frame
    } else {
        // Calculate mouse deltas
        float x_offset = input->mouse_x - input->last_mouse_x;
        float y_offset = input->last_mouse_y - input->mouse_y;
        
        // Update last mouse position for next frame
        input->last_mouse_x = input->mouse_x;
        input->last_mouse_y = input->mouse_y;

        // Only process if there's actual movement
        if (x_offset != 0.0f || y_offset != 0.0f)
            process_mouse_movement(&ctx->camera_data.camera, x_offset, y_offset);
    }

	if (input->scroll_delta != 0.0f) {
		process_mouse_scroll(&ctx->camera_data.camera, input->scroll_delta);
		display_zoom(&ctx->assets, &ctx->viewport, &ctx->camera_data);
	}

	// Processing camera mode switches between FPS and freefly
	if (update_camera_from_input(window, &ctx->camera_data, input, dt))
		display_camera_mode_status(&ctx->assets, ctx->camera_data.mode);


	// Handling render mode changes
	if (is_key_pressed(input, GLFW_KEY_UP)) {
		cycle_render_mode(&ctx->post_processing, true);
		display_render_mode_status(ctx);
	}

	if (is_key_pressed(input, GLFW_KEY_DOWN)) {
		cycle_render_mode(&ctx->post_processing, false);
		display_render_mode_status(ctx);
	}


	// Toggling debug mode (displaying information).
	if (is_key_pressed(input, GLFW_KEY_Q)) {
		ctx->debug_mode = !ctx->debug_mode;
		display_debug_mode_status(ctx);
	}
}


// The main function that will update the input state and handle inputs between
// the application states
void process_input(GLFWwindow* window, InputState* input, ApplicationState& app, Menu* menu, RenderingContext* context, float dt) {
	update_input_state(input, window);

	switch(app) {
	case ApplicationState::MENU:
		handle_menu_input(window, input, menu, app);
		break;
	case ApplicationState::GAME:
		handle_game_input(window, input, context, dt, app);
		break;
	}

	input->scroll_delta = 0.0f;
}


// Callback functions
void setup_input_callbacks(GLFWwindow* window, CallbackContext* context) {
	glfwSetWindowUserPointer(window, context);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, 	 scroll_callback);
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
	CallbackContext* ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));

	if (!ctx || !ctx->input_state) {
		return;
	}

	// Always update mouse position regardless of state
	InputState* input = ctx->input_state;
	input->mouse_x = x_pos;
	input->mouse_y = y_pos;
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
	CallbackContext* ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
    
    if (!ctx || !ctx->input_state || !ctx->app_state || *ctx->app_state != ApplicationState::GAME) {
        return;
    }
    
    ctx->input_state->scroll_delta = y_offset;
}

