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
#include "render_state.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//prototyping functions that will be declared beneath the main function
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);


/*
//window settings
const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 900;
const float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT;
*/


//Time variables
float delta_time;

//setting up camera
Camera camera(glm::vec3(0.0f, -4.0f, 5.0f));
float lastX = RenderState::SCREEN_WIDTH / 2;
float lastY = RenderState::SCREEN_HEIGHT / 2;
bool firstMouse = true;


// Game state
enum class ApplicationState {
	GAME,
	MENU
};


int main()
{
	std::cout << "OpenGL Version 4.6.0 - LearningOpenGL by McFlubberBubber.\n";

	// Initializing GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

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
	glfwSetCursorPosCallback(window, mouse_callback);					
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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


	ApplicationState current_state = ApplicationState::GAME;


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


	//-------------------------------- RENDER LOOP ----------------------------------------
	while (!glfwWindowShouldClose(window)) {		//checks if glfw has been instructed to close
		Time::update();
		delta_time = Time::get_delta_time();
		
		processInput(window, camera, delta_time, input_state);
		renderScene(camera);

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


//function to handle the camera looking around the scene
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) {
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
	camera.processMouseScroll(static_cast<float>(yOffset));
}
