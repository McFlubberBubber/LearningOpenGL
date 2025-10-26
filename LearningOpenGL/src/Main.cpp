#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "core/time.h"
#include "core/program_state.h"
#include "core/types.h"

#include "renderer/render_context.h"
#include "renderer/render_system.h"

#include "input/user_input.h"
#include "ui/menu.h"

#include "world/obj_init.h"
#include "world/scene.h"
#include "world/space_scene.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Internal function prototypes.
static void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height);

int main() {
	// Initializing structs...
	RenderingContext render_context = {};
	InputState input_state;
	Menu menu;
	SceneState prev_scene = render_context.app.scene;

	CallbackContext callback_context = {
	&render_context,
	&input_state,
	&menu
	};

	// Initializing the viewport for width and height parameters, then the application specs.
	init_viewport(&render_context.viewport);
	if (!init_application(&render_context.app, &render_context.viewport)) {
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
	update_mouse_flags(&input_state, mouse_x, mouse_y);

	stbi_set_flip_vertically_on_load(true);

	if (!init_rendering_system(&render_context)) {
		std::cout << "ERROR: Rendering system did not initialize!" << std::endl;
		return -1;
	}

	// INSTANCE RENDERING EXAMPLE
	render_context.buffers.update_instance_offsets();
	generate_rock_matrices(&render_context);
	generate_blahaj_matrices(&render_context);
//	validate_rock_instancing(&render_context); // Logging info.

	init_menu(&menu);

	// ----- RENDER LOOP -----
	while (!glfwWindowShouldClose(render_context.app.window)) {	
		check_for_window_updates(&render_context.app, &render_context.viewport); // @TODO: Important note in the function definition here!
		Time::update();
		float dt = Time::get_delta_time();

		// Handling cursor stuff
		if (render_context.app.scene != prev_scene) {

			if (render_context.app.scene != SceneState::MENU) {
				glfwGetCursorPos(render_context.app.window, &mouse_x, &mouse_y);
				update_mouse_flags(&input_state, mouse_x, mouse_y);

				glfwSetInputMode(render_context.app.window, GLFW_CURSOR,
								 GLFW_CURSOR_DISABLED);
			} else {
				glfwSetInputMode(render_context.app.window, GLFW_CURSOR,
								 GLFW_CURSOR_NORMAL);
			}

			prev_scene = render_context.app.scene;
		}

		process_input(render_context.app.window, &input_state, &menu, &render_context, dt);

		// @TODO: Since we are now rendering MULTIPLE scenes, it would make sense to structure them together to switch between the two,
		// but this is what we are doing for now to get it up and running.
		switch (render_context.app.scene) {
		case (SceneState::MAIN):
			render_scene(&render_context, dt);
			break;
		case (SceneState::MENU):
			draw_menu(&render_context, &menu);
			break;
		case(SceneState::SPACE):
			render_space_scene(&render_context, dt);
			break;
		}

		//checking call events and swapping buffers
		glfwSwapBuffers(render_context.app.window);
		glfwPollEvents();
	}
	
	cleanup_rendering_system(&render_context);
	cleanup_space_scene();
	cleanup_main_scene();
	glfwTerminate();

	return 0;
}


// ----- Internal Functions -----
static void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height) {
	CallbackContext* ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	
	glViewport(0, 0, width, height);
	if (ctx && ctx->render_context) {
		set_screen_size(&ctx->render_context->viewport, width, height);
		resize_framebuffer(ctx->render_context, width, height);
	}
}

