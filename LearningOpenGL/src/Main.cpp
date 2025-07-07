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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//prototyping functions that will be declared beneath the main function
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
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
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Learning OpenGL", NULL, NULL);
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
	stbi_set_flip_vertically_on_load(true);									// flipping all images loaded vertically				


	//ensuring that glad is initialized before we use openGL functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}

	// Configuring global openGL state
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Initializing here...
	initBuffers();
	initShaders();
	initModels();
	initTextures();

	//-------------------------------- RENDER LOOP ----------------------------------------
	while (!glfwWindowShouldClose(window)) {		//checks if glfw has been instructed to close
		Time::update();
		deltaTime = Time::getDeltaTime();
		currentTime = Time::getTime();
		counter++;

		processInput(window);

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
}

//function to handle user input
void processInput(GLFWwindow* window) {
	//variables for toggling between GL_LINE and GL_FILL
	static bool s_wireframeMode = false;
	static bool s_enterState = false;
	bool enterPressed = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;

	//variables for toggling between free fly / FPS mode
	static bool s_fpsMode = false;
	static bool s_eState = false;
	bool ePressed = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

	//if the user presses escape, close the window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//if the user presses enter, toggle wireframe mode
	if (enterPressed && !s_enterState)
	{
		s_wireframeMode = !s_wireframeMode;
		glPolygonMode(GL_FRONT_AND_BACK, s_wireframeMode ? GL_LINE : GL_FILL);
	}
	s_enterState = enterPressed;

	//if the user presses E, toggle between free fly / FPS camera
	if (ePressed && !s_eState) {
		s_fpsMode = !s_fpsMode;
		if (s_fpsMode) {
			std::cout << "FPS MODE ENABLED!" << std::endl;
			camera.position.y = -4.0f;
		} 
		else {
			std::cout << "FREE FLY MODE ENABLED!" << std::endl;
		}
	}
	s_eState = ePressed;

	if (s_fpsMode) {
		//camera movement inputs - FPS VERSION
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.processFPSMovement(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.processFPSMovement(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.processFPSMovement(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.processFPSMovement(RIGHT, deltaTime);
	}
	else {
		//camera movement inputs - FREE FLY
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.processMovement(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.processMovement(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.processMovement(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.processMovement(RIGHT, deltaTime);
	}
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

//function to handle zooming in and out
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.processMouseScroll(static_cast<float>(yOffset));
}