#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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


void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height);


// @TODO: Clean up the main function and move the GLFW / GLAD init stuff to a seperate file.
int main()
{
	std::cout << "OpenGL Version 4.6.0 - LearningOpenGL by McFlubberBubber.\n";
	u32 GL_MAJOR_VER = 4;
	u32 GL_MINOR_VER = 6;
	u32 GL_BABY_VER  = 0;

	// Initializing structs...
	ApplicationState app_state = ApplicationState::SCENE;
	RenderingContext render_context = {};
	InputState input_state;
	Menu menu;

	ApplicationState prev_app_state = app_state;
	render_context.app_state = &app_state;

	CallbackContext callback_context = {
	&render_context,
	&input_state,
	&menu,
	&app_state
	};

	// Setting up GLFWwindow
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR_VER);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR_VER);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	init_viewport(&render_context.viewport);
	GLFWwindow* window = glfwCreateWindow(render_context.viewport.width, render_context.viewport.height, "LearningOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to load GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 40);
	glfwMakeContextCurrent(window);

	// Calling this function whenever the user resizes window
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	setup_input_callbacks(window, &callback_context);

	// Initialize values for input_state.
	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	input_state.mouse_x = mouse_x;
	input_state.mouse_y = mouse_y;
	input_state.last_mouse_x = mouse_x;
	input_state.last_mouse_y = mouse_y;
	input_state.first_mouse = true;
	
	if (app_state == ApplicationState::SCENE || app_state == ApplicationState::SPACE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	// @HARDCODE: V-SYNC ENABLER (0: off / 1: on)
	glfwSwapInterval(1);
	stbi_set_flip_vertically_on_load(true);

	//ensuring that glad is initialized before we use openGL functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}

	// OpenGL Global Configuration	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);

	// INSTANCE RENDERING EXAMPLE
	render_context.buffers.update_instance_offsets();
	generate_rock_matrices();

	// Initializing here...
	if (!init_rendering_system(&render_context)) {
		std::cout << "ERROR: Rendering system did not initialize!" << std::endl;
		return -1;
	}
	
	init_menu(&menu);
	init_world_objects(&render_context.world);
	init_lighting(&render_context.lighting);

	// ----- RENDER LOOP -----
	while (!glfwWindowShouldClose(window)) {	
		Time::update();
		float dt = Time::get_delta_time();

		// Handling input states
		if (app_state != prev_app_state) {
			if (app_state != ApplicationState::MENU) {
				glfwGetCursorPos(window, &mouse_x, &mouse_y);
				input_state.mouse_x = mouse_x;
				input_state.mouse_y = mouse_y;
				input_state.last_mouse_x = mouse_x;
				input_state.last_mouse_y = mouse_y;
				input_state.first_mouse = true;

				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			} else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			prev_app_state = app_state;
		}

		process_input(window, &input_state, app_state, &menu, &render_context, dt);

		// @TODO: Since we are now rendering MULTIPLE scenes, it would make sense to structure them together to switch between the two,
		// but this is what we are doing for now to get it up and running.
		switch (app_state) {
		case (ApplicationState::SCENE):
			render_scene(&render_context, dt);
			break;
		case (ApplicationState::MENU):
			draw_menu(&render_context, &menu);
			break;
		case(ApplicationState::SPACE):
			render_space_scene(&render_context, dt);
			break;
		}

		//checking call events and swapping buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	cleanup_rendering_system(&render_context);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height) {
	CallbackContext* ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	
	glViewport(0, 0, width, height);
	if (ctx && ctx->render_context) {
		set_screen_size(&ctx->render_context->viewport, width, height);
		resize_framebuffer(ctx->render_context, width, height);
	}
}

