#pragma once

#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "core/types.h"

//#include "renderer/post_processing.h"
//#include "renderer/render_context.h"
//#include "core/program_state.h"
//#include "ui/menu.h"

// Forward declarations
struct RenderingContext;
struct Menu;
struct Console;

struct InputState {
	std::unordered_map<s32, bool> key_states;		// Current frame state
	std::unordered_map<s32, bool> last_key_states;	// Last frame state

	double mouse_x = 0.0;
	double mouse_y = 0.0;
	double last_mouse_x = 0.0;
	double last_mouse_y = 0.0;

	double scroll_delta = 0.0;

	bool first_mouse = true;
};

struct CallbackContext {
	RenderingContext* render_context;
	InputState* input_state;
	Menu* menu;
	Console* console;
};

// Reading user input
inline bool is_key_down(InputState* input, s32 key) {				// Held
	return input->key_states[key];
}

inline bool is_key_pressed(InputState* input, s32 key) {			// Pressed once
	return input->key_states[key] && !input->last_key_states[key];
}

inline bool is_key_released(InputState* input, s32 key) {			// Released
	return !input->key_states[key] && input->last_key_states[key];
}

void reset_mouse_tracking(InputState *input, double mouse_x, double mouse_y);
void update_mouse_position(InputState* input, double mouse_x, double mouse_y);

// Functions to handle user input in different contexts
void process_input(GLFWwindow* window, InputState* input_state, Menu* menu, RenderingContext* context, float dt, Console* console);

// Callback functions
void setup_input_callbacks(GLFWwindow* window, CallbackContext* context);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offet, double y_offset);
void key_callback(GLFWwindow* window, int key, int scan_code, int action, int mods);
void character_callback(GLFWwindow* window, u32 codepoint);
