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

#if 0
	// Updating current states
	for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++) {
		int state = glfwGetKey(window, key);
		input->key_states[key] = (state == GLFW_PRESS);
	}
#endif

	// Only check for movement keys.
	static const int movement_keys[] = {
		GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D,
		GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_E,
		// We can add more keys here that need CONTINOUS checking.
	};

	for (int key : movement_keys) {
		int state = glfwGetKey(window, key);
		input->key_states[key] = (state == GLFW_PRESS);
	}

//	input->scroll_delta = 0.0f;
}

// @REFACTOR: Handling menu inputs.
static void handle_menu_key_input(Menu* menu, RenderingContext* ctx, int key, int mods) {
	auto& scene = ctx->app.scene;

	switch(key) {
	// Handling menu navigations.
	case GLFW_KEY_UP: {
		decrement_menu_item(menu);
		break;
	}
	case GLFW_KEY_DOWN: {
		increment_menu_item(menu);
		break;
	}

	// Escape button behaviour.
	case GLFW_KEY_ESCAPE: {
		if (menu->current_page == MenuPage::MAIN) { // Go back to game when in main.
			scene = ctx->app.prev_scene;
		} else if (menu->current_page == MenuPage::OPTIONS) { // Else, go back to main.
			menu->current_page = MenuPage::MAIN;
			menu->main.current_item = MenuItem::RESUME;
		}
		break;
	}

	// Enter key behaviour.
	case GLFW_KEY_ENTER: {
		if (menu->current_page == MenuPage::MAIN) {
			handle_main_menu_activations(menu, ctx);
		} else if (menu->current_page == MenuPage::OPTIONS) {
			handle_options_menu_activations(menu, ctx);
		}
		break;
	}
	}
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

static void handle_game_key_input(RenderingContext* ctx, Menu* menu, int key, int mods) {
	auto& scene = ctx->app.scene;

	switch (key) {
	// Handling ESCAPE to go to the menu.
	case GLFW_KEY_ESCAPE: {
		scene = SceneState::MENU;
		menu->current_page = MenuPage::MAIN;
		menu->main.current_item = MenuItem::RESUME;
		break;
	}

	// Render mode cycling (post-processing).
	case GLFW_KEY_UP: {
		cycle_render_mode(&ctx->post_processing, true);
		display_render_mode_status(ctx);
		break;
	}
	case GLFW_KEY_DOWN: {
		cycle_render_mode(&ctx->post_processing, false);
		display_render_mode_status(ctx);
		break;
	}

	// Debug mode toggle.
	case GLFW_KEY_Q: {
		ctx->app.config.debug_mode = !ctx->app.config.debug_mode;
		display_debug_mode_status(ctx);
		break;
	}

	}
}

#if 0
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
#endif

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
	// Only poll the keys that will be used in the game continously.
	update_input_state(input, window);

	// If the console is open, only handle mouse movement.
	if (console->state != ConsoleState::CLOSED) {
		do_mouse_movement(input, ctx);
		input->scroll_delta = 0.0f;
		return;
	}

	// Menu does not require continous input, probably.
	if (ctx->app.scene == SceneState::MENU) {
		input->scroll_delta = 0.0f;
		return;
	}

	// Game state does require some continous input, therefore...
	do_mouse_movement(input, ctx);
	if (input->scroll_delta != 0.0f) {
		process_mouse_scroll(&ctx->camera_data.camera, (float)input->scroll_delta);
		display_zoom(&ctx->assets, &ctx->viewport, &ctx->camera_data);
	}

	if (update_camera_from_input(window, &ctx->camera_data, input, dt)) {
		display_camera_mode_status(&ctx->assets, ctx->camera_data.mode);
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
	CallbackContext* callback_ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	if (!callback_ctx || !callback_ctx->input_state || !callback_ctx->console) {
		return;
	}

	RenderingContext* render_ctx = callback_ctx->render_context;
	InputState* input	  		 = callback_ctx->input_state;
	Menu* menu 			  		 = callback_ctx->menu;
	Console* console      		 = callback_ctx->console;

	bool is_pressed = (action == GLFW_PRESS);
	bool is_pressed_or_repeated = (action == GLFW_PRESS || action == GLFW_REPEAT);

	// Checking for console activations.
	if (key == GLFW_KEY_GRAVE_ACCENT && is_pressed) {
		// Opening console in BIGMODE!
		if (mods & GLFW_MOD_SHIFT) {
			if (console->state != ConsoleState::OPEN_BIG) {
				console->state = ConsoleState::OPEN_BIG;
				push_message(&render_ctx->message_queue, "Opening big console");
			} else {
				console->state = ConsoleState::CLOSED;
				push_message(&render_ctx->message_queue, "Closing big console");
			}
			
		} else { // Otherwise, open console in small mode...
			if (console->state != ConsoleState::OPEN_SMALL) {
				console->state = ConsoleState::OPEN_SMALL;
				push_message(&render_ctx->message_queue, "Opening small console");
			} else {
				console->state = ConsoleState::CLOSED;
				push_message(&render_ctx->message_queue, "Closing small console");
			}
		}
		return;
	}

	// Handling special key inputs within the console.
	if (console->state != ConsoleState::CLOSED) {
		handle_console_key_input(console, key, scan_code, action, mods);
		return;
	}

	// Handling menu input.
	if (render_ctx->app.scene == SceneState::MENU && is_pressed) {
		handle_menu_key_input(menu, render_ctx, key, mods);
		return;
	}

	// Handling game input.
	if (is_pressed) {
		handle_game_key_input(render_ctx, menu, key, mods);
	}
}

// This callback is mainly here to take inputs for the console.
void character_callback(GLFWwindow* window, u32 codepoint) {
	CallbackContext* callback_ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	if (!callback_ctx || !callback_ctx->input_state || !callback_ctx->console) {
		return;
	}

	Console* console = callback_ctx->console;
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

