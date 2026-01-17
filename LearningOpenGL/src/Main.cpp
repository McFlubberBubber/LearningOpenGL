#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "core/time.h"
#include "core/program_state.h"
#include "core/types.h"
#include "core/hotloader.h"
#include "core/vars.h"

#include "renderer/render_context.h"
#include "renderer/render_system.h"

#include "input/user_input.h"
#include "ui/menu.h"
#include "ui/console.h"

#include "world/obj_init.h"
#include "world/scene.h"
#include "world/space_scene.h"
#include "world/shadow_scene.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Internal function prototypes.
static void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height);

int main() {
	// Initializing structs...
	RenderingContext render_context = {};
	InputState input_state;
	
	Menu menu;
	Console console;

	SceneState prev_scene = render_context.app.scene;
	render_context.app.prev_scene = render_context.app.scene;

	CallbackContext callback_context = {
	&render_context,
	&input_state,
	&menu,
	&console
	};

	bool app_status = init_application(&render_context.app, &render_context.viewport);
	assert(app_status);
	if (!app_status) {
		std::cout << "Failed to initialize application!" << std::endl;
		return -1;
	} else {
		std::cout << "Finished initializing application!" << std::endl;
	}

	// Calling this function whenever the user resizes window
	glfwSetFramebufferSizeCallback(render_context.app.window, framebuffer_size_callback);
	setup_input_callbacks(render_context.app.window, &callback_context);

	// Initialize values for input_state.
	double mouse_x, mouse_y;
	glfwGetCursorPos(render_context.app.window, &mouse_x, &mouse_y);
	reset_mouse_tracking(&input_state, mouse_x, mouse_y);

	stbi_set_flip_vertically_on_load(true);

	bool renderer_status = init_rendering_system(&render_context);
	assert(renderer_status);
	if (!renderer_status) {
		std::cout << "ERROR: Rendering system did not initialize!" << std::endl;
		return -1;
	}

	// INSTANCE RENDERING EXAMPLE
	render_context.buffers.update_instance_offsets();
	generate_rock_matrices(&render_context);
	generate_blahaj_matrices(&render_context);
//	validate_rock_instancing(&render_context); // Logging info.

	init_menu(&menu, &render_context);
	init_console(&console, &render_context);

	// Hotloader stuff...
	Hotloader hotloader = {};
	const char* vars_path = "config/hotloaded.variables";
	init_hotloader(&hotloader, vars_path);
	init_vars(&render_context.vars, vars_path, &console);

	// Initializing scenes, if needed.
	init_shadow_scene(&render_context);

	// ----- RENDER LOOP -----
	while (render_context.app.is_running) {	
		// @TODO: Important note in the function definition here!
		check_for_window_updates(&render_context.app, &render_context.viewport);
		update_hotloader(&hotloader, &render_context);
			
		Time::update();
		float dt = Time::get_delta_time();

		glfwGetCursorPos(render_context.app.window, &mouse_x, &mouse_y);
		update_mouse_position(&input_state, mouse_x, mouse_y);
		
		static bool was_menu_open = false;
		if (render_context.app.is_menu_open) {
			if (!was_menu_open) {
				reset_mouse_tracking(&input_state, mouse_x, mouse_y);
			}
			glfwSetInputMode(render_context.app.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		} else {
			if (was_menu_open) {
				reset_mouse_tracking(&input_state, mouse_x, mouse_y);
			}
			glfwSetInputMode(render_context.app.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		was_menu_open = render_context.app.is_menu_open;

		process_input(render_context.app.window, &input_state, &menu, &render_context, dt, &console);

		switch (render_context.app.scene) {
		case (SceneState::MAIN):
			render_scene(&render_context, dt);
			break;
		case(SceneState::SPACE):
			render_space_scene(&render_context, dt);
			break;
		case(SceneState::SHADOW):
			render_shadow_scene(&render_context, dt);
			break;
		}

		if (render_context.app.is_menu_open) {
			draw_menu(&menu, &render_context);
		}

		// We could branch this out so that this function call never happens, but there is a check
		// already within this function.
		draw_console(&console, &render_context);
		
		glfwSwapBuffers(render_context.app.window);
		glfwPollEvents();
	}
	
	update_config_from_app(&render_context.app);
	cleanup_rendering_system(&render_context);
	cleanup_space_scene();
	cleanup_main_scene();
	cleanup_hotloader(&hotloader);
	
	glfwTerminate();

	return 0;
}


// ----- Internal Functions -----
static void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height) {
	CallbackContext* ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	
	glViewport(0, 0, width, height);
	if (ctx && ctx->render_context) {
		set_screen_size(&ctx->render_context->viewport, &ctx->render_context->app,
						width, height);
		resize_framebuffer(ctx->render_context, width, height);
	}
}

