#include "user_input.h"

#include <iostream>

#include "renderer/render_context.h"
#include "renderer/render_system.h"

#include "core/program_state.h"
#include "core/time.h"

#include "ui/debug_overlay.h"
#include "ui/menu.h"
#include "ui/console.h"

void update_mouse_flags(InputState* input, double mouse_x, double mouse_y) {
	input->mouse_x = mouse_x;
	input->mouse_y = mouse_y;
	input->last_mouse_x = mouse_x;
	input->last_mouse_y = mouse_y;

	input->first_mouse = true;
}

static void update_input_state(InputState* input, GLFWwindow* window) {
	// Copy current state into the last
	input->last_key_states = input->key_states;

	// Updating current states
	for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++) {
		int state = glfwGetKey(window, key);
		input->key_states[key] = (state == GLFW_PRESS);
	}

//	input->scroll_delta = 0.0f;
}

static void handle_menu_input(GLFWwindow* window, InputState* input, Menu* menu, RenderingContext* ctx,	SceneState& prev_scene) {
	auto& scene = ctx->app.scene;

	// Handling inputs for main page
	if (menu->current_page == MenuPage::MAIN) {

		// Allowing user to go back to game state
		if (is_key_pressed(input, GLFW_KEY_ESCAPE) && scene == SceneState::MENU) {
			scene = prev_scene;
		}

		// Checking for activations
		if (is_key_pressed(input, GLFW_KEY_ENTER)) {
		
			// Main page inputs
			switch (menu->main.current_item) {
			case MenuItem::RESUME:
				scene = prev_scene;
				break;
		
			case MenuItem::OPTIONS:
				menu->current_page = MenuPage::OPTIONS;
				menu->options.current_item = OptionsItem::MUSIC; // Resetting
				break;

			case MenuItem::SCENE_SWITCH:
				menu->render_normal_scene = !menu->render_normal_scene;
				if (menu->render_normal_scene) {
					scene = SceneState::MAIN;
					display_current_scene_status(ctx);
				}
				else {
					scene = SceneState::SPACE;
					display_current_scene_status(ctx);
				}
				break;

			case MenuItem::QUIT:
				glfwSetWindowShouldClose(window, true);
				break;
		
			default:
				break;
			}
		}
	}
	
	// Handling inputs for options page
	else if (menu->current_page == MenuPage::OPTIONS) {

		// Allowing the user to go back a page
		if (is_key_pressed(input, GLFW_KEY_ESCAPE)) {
			menu->current_page = MenuPage::MAIN;
			menu->main.current_item = MenuItem::RESUME; // Resetting 
		}

		// Checking for activations (with ENTER key)
		if (is_key_pressed(input, GLFW_KEY_ENTER)) {
			std::string message;

			switch (menu->options.current_item) {
			case OptionsItem::DISPLAY:
				ctx->app.config.fullscreen = !ctx->app.config.fullscreen;
				menu->do_fullscreen = ctx->app.config.fullscreen;

				message = std::string("Changed display to ") +
					(menu->do_fullscreen ? "fullscreen" : "windowed") +
					" mode.";
				push_message(&ctx->message_queue, message);
				break;

			case OptionsItem::MUSIC:
				menu->do_music = !menu->do_music;
				
				message = std::string("Music toggled to ") + (menu->do_music ? "ON" : "OFF") + " state.";
				push_message(&ctx->message_queue, message);
				break;

			case OptionsItem::VSYNC:
				ctx->app.config.vsync = !ctx->app.config.vsync;
				menu->do_vsync = ctx->app.config.vsync;
				
				message = std::string("Vsync ") + (ctx->app.config.vsync ? "Enabled!" : "Disabled!");
				push_message(&ctx->message_queue, message);
				break;


			// @TODO: Update screen shader to use a uniform to read gamma flags +
			// clamp values from 0.1f to 1.0f (gamma goes from 0.1 to 2.2)
			case OptionsItem::GAMMA:
				// @TODO: Use arrow keys to handle slider?
				push_message(&ctx->message_queue, "Gamma slider not implemented!");
				break;

			// @TODO: Fix the buffers in BufferData to implement this.
			case OptionsItem::MULTISAMPLING:
				menu->do_multisampling = !menu->do_multisampling;

				message = std::string("Multisampling toggle not implemented!");
				push_message(&ctx->message_queue, message);
				break;

			case OptionsItem::BACK:
				menu->current_page = MenuPage::MAIN;
				menu->main.current_item = MenuItem::RESUME; // Resetting
				break;
				
			default:
				break;
			}
		}
	}

	// These inputs are read regardless of what page we are in
	// therefore these will always be checked.
	if (is_key_pressed(input, GLFW_KEY_UP))
		decrement_menu_item(menu);

	if (is_key_pressed(input, GLFW_KEY_DOWN))
		increment_menu_item(menu);
}

static void do_mouse_movement(InputState* input, RenderingContext* ctx) {
    // Handle first mouse movement to prevent camera jump
    if (input->first_mouse) {
        input->last_mouse_x = input->mouse_x;
        input->last_mouse_y = input->mouse_y;
        input->first_mouse = false;
        // Skip mouse processing this frame
    } else {
        // Calculate mouse deltas
        float x_offset = (float)input->mouse_x - (float)input->last_mouse_x;
        float y_offset = (float)input->last_mouse_y - (float)input->mouse_y;
        
        // Update last mouse position for next frame
        input->last_mouse_x = input->mouse_x;
        input->last_mouse_y = input->mouse_y;

        // Only process if there's actual movement
        if (x_offset != 0.0f || y_offset != 0.0f)
            process_mouse_movement(&ctx->camera_data.camera, x_offset, y_offset);
    }
}

static void handle_game_input(GLFWwindow* window, InputState* input, Menu* menu, RenderingContext* ctx, float dt) {
	auto& scene = ctx->app.scene; // Make sure to get the reference of it so that we are referring to the app_state

	// Allowing the user to switch to menu
	if (is_key_pressed(input, GLFW_KEY_ESCAPE) && scene != SceneState::MENU) {
		scene = SceneState::MENU;
		menu->current_page = MenuPage::MAIN;
		menu->main.current_item = MenuItem::RESUME;
	}
	
	do_mouse_movement(input, ctx);
	
	if (input->scroll_delta != 0.0f) {
		process_mouse_scroll(&ctx->camera_data.camera, (float)input->scroll_delta);
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
		ctx->app.config.debug_mode = !ctx->app.config.debug_mode;
		display_debug_mode_status(ctx);
	}
}


static void handle_console_key_input(Console* console, int key, int scan_code, int action, int mods) {
	// To ensure we handle only presses and repeats.
	if (action != GLFW_PRESS && action != GLFW_REPEAT) { return; }

	switch (key) {
	case GLFW_KEY_ENTER: {
		// execute_command(console);
		break;
	}
		
	case GLFW_KEY_BACKSPACE: {
		// delete_char(console);
		break;
	}
		
	case GLFW_KEY_ESCAPE: {
		// idk what to do here, maybe an alternative to closing the console?
		break;
	}
		
	// Maybe these cases can be used to scroll through the history of the logs?
	case GLFW_KEY_UP: {
		break;
	}
	case GLFW_KEY_DOWN: {
		break;
	}
		
	case GLFW_KEY_TAB: {
		// autocomplete_command(console);
		break;
	}

	}
}


// The main function that will update the input state and handle inputs between
// the application states
void process_input(GLFWwindow* window, InputState* input, Menu* menu, RenderingContext* ctx, float dt, Console* console) {
	update_input_state(input, window);

	// Storing the previous app state before the menu state.
	static SceneState prev_scene = ctx->app.scene;
	if (ctx->app.scene != SceneState::MENU) {
		prev_scene = ctx->app.scene;
	}

	if (console->state != ConsoleState::CLOSED) {
		do_mouse_movement(input, ctx); // We still want to handle mouse movements.
		input->scroll_delta = 0.0f;
		return;
	}
	
	// Processing different input systems based on the app state.
	switch(ctx->app.scene) {
	case SceneState::MENU:
		handle_menu_input(window, input, menu, ctx, prev_scene);
		break;
	case SceneState::MAIN:
		handle_game_input(window, input, menu, ctx, dt);
		break;
	case SceneState::SPACE:
		handle_game_input(window, input, menu, ctx, dt);
		break;
	}

	input->scroll_delta = 0.0f;
}


// Callback functions
void setup_input_callbacks(GLFWwindow* window, CallbackContext* context) {
	glfwSetWindowUserPointer(window, context);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, 	 scroll_callback);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);
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

	if (!ctx || !ctx->input_state) {
		return;
	}

	ctx->input_state->scroll_delta = y_offset;
}

// This function will allow us to check for console activations, and read special char inputs when
// we take input for the console.
void key_callback(GLFWwindow* window, int key, int scan_code, int action, int mods) {
	// Handling special key presses (shift, backspace, enter...)
	CallbackContext* ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	if (!ctx || !ctx->input_state || !ctx->console) {
		return;
	}

	InputState* input = ctx->input_state;
	Console* console  = ctx->console;

	// Checking for console activations.
	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {

		// Opening console in BIGMODE!
		if (mods & GLFW_MOD_SHIFT) {
			if (console->state != ConsoleState::OPEN_BIG) {
				console->state = ConsoleState::OPEN_BIG;
				push_message(&ctx->render_context->message_queue, "Opening big console");
			} else {
				console->state = ConsoleState::CLOSED;
				push_message(&ctx->render_context->message_queue, "Closing big console");
			}
			
		} else { // Otherwise, open console in small mode...
			if (console->state != ConsoleState::OPEN_SMALL) {
				console->state = ConsoleState::OPEN_SMALL;
				push_message(&ctx->render_context->message_queue, "Opening small console");
			} else {
				console->state = ConsoleState::CLOSED;
				push_message(&ctx->render_context->message_queue, "Closing small console");
			}
		}
		return;
	}

	// Handling special key inputs.
	if (console->state != ConsoleState::CLOSED) {
		handle_console_key_input(console, key, scan_code, action, mods);
		return;
	}
	
}

// This callback is mainly here to take inputs for the console.
void character_callback(GLFWwindow* window, u32 codepoint) {
	CallbackContext* ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	if (!ctx || !ctx->input_state || !ctx->console) {
		return;
	}

	Console* console = ctx->console;
	if (console->state == ConsoleState::CLOSED) {
		return; // We only want to process this callback if the console is open.
	}

	//
	// @TODO: Append the character press to the console here.
	// 

	// Debug output (for now).
	if (codepoint < 128) {
		std::cout << "Console received: '" << static_cast<char>(codepoint) << "'\n";
	}
}

