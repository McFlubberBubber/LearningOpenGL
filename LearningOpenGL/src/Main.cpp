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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//prototyping functions that will be declared beneath the main function
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

//window settings
const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;
const float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT;

//Time variables
float deltaTime;
float currentTime;
static float timeAccumulator; 
unsigned int counter;

//setting up camera
Camera camera(glm::vec3(0.0f, -4.0f, 5.0f));
float lastX = SCREEN_WIDTH / 2;
float lastY = SCREEN_HEIGHT / 2;
bool firstMouse = true;



int main()
{
	//initializing GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					//setting major version 3.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					//setting minor version 0.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//using core profile

	//setting up GLFWwindow
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearningOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to load GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);											// setting the current context to the window 	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);		// calling this function whenever the user resizes window
	glfwSetCursorPosCallback(window, mouse_callback);						// calling the mouse callback to handle looking around
	glfwSetScrollCallback(window, scroll_callback);							// calling scroll to allow zooming within the scene
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);			// capturing our mouse
	glfwSwapInterval(1);													// this line enables v-sync
	stbi_set_flip_vertically_on_load(true);


	//ensuring that glad is initialized before we use openGL functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}

	
	// Configuring global openGL state
	/*
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);	
	*/

	/*
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);		// Incase we prefer clockwise faces instead of counter clockwise
	*/
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	// @TODO All these functions can be shrunk down to an initGame() or initScene()
	// but for now, it's nice to know what exactly we are initializing
	// Initializing here...
	initBuffers(SCREEN_WIDTH, SCREEN_HEIGHT);
	initShaders();
	initModels();
	initTextures();

	init_skybox();

	// @NOTE This can proably be done under an init function aswell
	// but this is here just to test the processInput function if it
	// works
	InputState input_state;

	//-------------------------------- RENDER LOOP ----------------------------------------
	while (!glfwWindowShouldClose(window)) {		//checks if glfw has been instructed to close
		Time::update();
		deltaTime = Time::getDeltaTime();
		currentTime = Time::getTime();
		counter++;

		processInput(window, camera, deltaTime, input_state);

		//FPS counter
		timeAccumulator += deltaTime;
		if (timeAccumulator >= 1.0f) {
			std::string fps = std::to_string(counter);
			std::string ms = std::to_string(1000.0f / (float)counter);
			std::string applicationTitle = "LearningOpenGL: " + fps + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, applicationTitle.c_str());
			counter = 0;
			timeAccumulator = 0.0f;
		}

		renderScene(camera, ASPECT_RATIO);

		//checking call events and swapping buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	
	cleanupScene();
	glfwTerminate();		//clearing resources that were allocated
	return 0;
}

//ensuring the viewport gets resized if the user does so
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
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
