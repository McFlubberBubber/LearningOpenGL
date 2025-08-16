#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Rendering.h"
#include "Time.h"
#include "UserInput.h"
#include "text_rendering.h"
#include "program_state.h"
#include "menu.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//prototyping functions that will be declared beneath the main function
void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);


//Time variables
float delta_time;

//setting up camera
Camera camera(glm::vec3(0.0f, -4.0f, 5.0f));
float lastX = RenderState::SCREEN_WIDTH / 2;
float lastY = RenderState::SCREEN_HEIGHT / 2;
bool firstMouse = true;

Menu menu;
 

int main()
{
	std::cout << "OpenGL Version 4.6.0 - LearningOpenGL by McFlubberBubber.\n";
	int32_t GL_MAJOR_VER = 4;
	int32_t GL_MINOR_VER = 6;
	int32_t GL_BABY_VER  = 0;

	ApplicationState app_state = ApplicationState::GAME;
	ApplicationState prev_app_state = app_state;
	
	// Initializing GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR_VER);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR_VER);

	// Using core profile instead of immediate
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Setting up GLFWwindow
	RenderState::set_screen_size(1600, 900);
	GLFWwindow* window = glfwCreateWindow(RenderState::SCREEN_WIDTH, RenderState::SCREEN_HEIGHT, "LearningOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to load GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Calling this function whenever the user resizes window
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Capturing mouse, and enabling cursor pos + zoom
	glfwSetWindowUserPointer(window, &app_state);

	if (app_state == ApplicationState::GAME) {
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
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

	
	// Configuring global openGL state
	/*
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);		// Incase we prefer clockwise faces instead of counter clockwise
	*/
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);





	// @TODO All these functions can be shrunk down to an initGame() or initScene()
	// but for now, it's nice to know what exactly we are initializing
	// Initializing here...
	initBuffers();
	initShaders();
	initModels();
	initTextures();
	init_fonts();

	init_skybox();
	init_reflection_cube();
	init_refraction_cube();

	InputState input_state;
	

	// Menu things
	init_menu();


	//-------------------------------- RENDER LOOP ----------------------------------------
	while (!glfwWindowShouldClose(window)) {		//checks if glfw has been instructed to close
		Time::update();


		if (app_state != prev_app_state) {
			if (app_state == ApplicationState::GAME) {
				firstMouse = true;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPosCallback(window, mouse_callback);
				glfwSetScrollCallback(window, scroll_callback);
			} else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				glfwSetCursorPosCallback(window, nullptr);
				glfwSetScrollCallback(window, nullptr);
			}
			prev_app_state = app_state;
		}


		if (app_state == ApplicationState::MENU) {
			process_menu_navigation(window, input_state, app_state, menu);
			draw_menu();
		} else {
			process_input(window, camera, input_state, app_state);
			render_scene(camera);
		}

		//checking call events and swapping buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	cleanupScene();
	glfwTerminate();		//clearing resources that were allocated
	return 0;
}

//ensuring the viewport gets resized if the user does so
void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
	glViewport(0, 0, width, height);
	RenderState::set_screen_size(width, height);
	resize_framebuffer(width, height);
}



// @TODO: With both of these callback functions, we store the last position of the
// cursor still, therefore we need to avoid making the cursor in the MENU state
// affect the camera mouse movement in the GAME state

//function to handle the camera looking around the scene
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) {
	ApplicationState* state = static_cast<ApplicationState*> (glfwGetWindowUserPointer(window));

	// Do not process mouse input 
	if (!state || *state != ApplicationState::GAME) {
		return;
	}
	
	
	float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);

	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;		//reversed since it ranges from bottom to top
	lastX = xPos;
	lastY = yPos;
	camera.processMouseMovement(xOffset, yOffset);
}


// @TODO The zoom currently breaks when the user resizes the window and thus
// only zooms onto one region of the screen
//function to handle zooming in and out
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	ApplicationState* state = static_cast<ApplicationState*> (glfwGetWindowUserPointer(window));

	// Do not process mouse input 
	if (!state || *state != ApplicationState::GAME) {
		return;
	}


	camera.processMouseScroll(static_cast<float>(yOffset));
}
