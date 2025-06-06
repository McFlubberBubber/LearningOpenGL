#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "Shader.h"
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//prototyping functions that will be declared beneath the main function
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
unsigned int loadTexture(const char* path);

//window settings
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;
const float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT;

float deltaTime = 0.0f;		//time between current and last frame
float lastFrame = 0.0f;		//time of last frame

//global variable that positions the light - can use vec4's w component to check if light is a position or direction (1.0f = position)
glm::vec3 lightDirection(1.2f, 3.0f, 2.0f);

//reusable light properties for directional lighting
glm::vec3 dirLightAmbient(0.0f);
glm::vec3 dirLightDiffuse(0.05f);
glm::vec3 dirLightSpecular(0.2f);

//coloring for each point light
glm::vec3 pointLightColors[]{
	glm::vec3(0.5f, 0.5f, 0.5f),
	glm::vec3(0.75f, 0.0f, 0.60f),
	glm::vec3(0.0f, 0.0f, 0.8f),
	glm::vec3(0.75f, 0.05f, 0.05f)
};

//setting up camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
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
	glfwMakeContextCurrent(window);											//setting the current context to the window 	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);		//calling this function whenever the user resizes window
	glfwSetCursorPosCallback(window, mouse_callback);						//calling the mouse callback to handle looking around
	glfwSetScrollCallback(window, scroll_callback);							//calling scroll to allow zooming within the scene
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);			//capturing our mouse

	//ensuring that glad is initialized before we use openGL functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}

	//enabling depth testing for z buffers
	glEnable(GL_DEPTH_TEST);

	//building and compiling the shader program (pathing starts from the solution directory)
	Shader containerShader("res/shaders/container.vert", "res/shaders/container.frag");
	Shader wallShader("res/shaders/wall.vert", "res/shaders/wall.frag");
	Shader lightingShader("res/shaders/container.vert", "res/shaders/lighting.frag");
	Shader lightCubeShader("res/shaders/container.vert", "res/shaders/lightCube.frag");

	//cube data
	float cubeVertices[] = {			//with positions, normals and textures
		// Back face
		-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		// Front face
		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		// Left face
		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f, 0.0f,   1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f, 0.0f,   1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f, 0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f, 0.0f,   1.0f, 0.0f,

		// Right face
		 0.5f,  0.5f,  0.5f,   1.0f,  0.0f, 0.0f,   1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f,  0.0f, 0.0f,   1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,   1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,   1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   1.0f,  0.0f, 0.0f,   0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   1.0f,  0.0f, 0.0f,   1.0f, 0.0f,

		 // Bottom face
		 -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
		  0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
		  0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		  0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		 -0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		 -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,

		 // Top face
		 -0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f,
		  0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
		  0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
		 -0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f
	};

	//creating the WORLD positions for x cubes that will be rendered - RANDOM POSITIONS
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	//world positions for point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	//creating an array of vertex objects
	unsigned int VAO[2], VBO[2], EBO;
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);


	//------- 3D RENDERING (CONTAINER)-------
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/*
	//------- 2D RENDERING (WALL) -------
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndices), wallIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	*/

	//loading textures
	unsigned int texture1 = loadTexture("res/textures/container.jpg");
	unsigned int texture2 = loadTexture("res/textures/awesomeface.png");
	unsigned int texture3 = loadTexture("res/textures/wall.jpg");
	unsigned int diffuseMap = loadTexture("res/textures/container2.png");
	unsigned int specularMap = loadTexture("res/textures/container2_specular.png");
	unsigned int emissionMap = loadTexture("res/textures/matrix.jpg");

	//setting texture uniforms
	containerShader.useProgram();
	containerShader.setInt("material.diffuse", 0);
	containerShader.setInt("material.specular", 1);

	//-------------------------------- RENDER LOOP ----------------------------------------
	while (!glfwWindowShouldClose(window))		//checks if glfw has been instructed to close
	{
		//per frame
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//getting user input through the application loop
		processInput(window);

		//rendering stuff will go here...
		glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//clearing the buffers every iteration

		//creating matrixes
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.zoom), ASPECT_RATIO, 0.1f, 100.0f);		//radians = FOV, width/height (aspect ratio), near place and far plane	
		glm::mat4 cameraView = camera.GetViewMatrix();

		//rendering 3D cubes
		containerShader.useProgram();
		//textures for containers
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		//applying matrixes
		containerShader.setVec3("u_viewPosition", camera.position);
		containerShader.setMat4("u_projectionMatrix", projectionMatrix);
		containerShader.setMat4("u_viewMatrix", cameraView);
		//setting material properties
		containerShader.setFloat("u_material.shininess", 32.0f);

		// ---------- LIGHT PROPERTIES OF CONTAINER SHADER ---------- 
		
		
		//DIRECTIONAL LIGHTING	
		containerShader.setVec3("u_dirLight.direction", lightDirection);
		containerShader.setVec3("u_dirLight.ambient", dirLightAmbient);
		containerShader.setVec3("u_dirLight.diffuse", dirLightDiffuse);
		containerShader.setVec3("u_dirLight.specular", dirLightSpecular);
		

		//POINT LIGHTING 1
		containerShader.setVec3("u_pointLight[0].position", pointLightPositions[0]);
		containerShader.setVec3("u_pointLight[0].ambient", pointLightColors[0].x * 0.1f, pointLightColors[0].y * 0.1f, pointLightColors[0].z * 0.1f);
		containerShader.setVec3("u_pointLight[0].diffuse", pointLightColors[0]);
		containerShader.setVec3("u_pointLight[0].specular", pointLightColors[0]);

		containerShader.setFloat("u_pointLight[0].constant", 1.0f);
		containerShader.setFloat("u_pointLight[0].linear", 0.09f);
		containerShader.setFloat("u_pointLight[0].quadratic", 0.032f);

		//POINT LIGHTING 2
		containerShader.setVec3("u_pointLight[1].position", pointLightPositions[1]);
		containerShader.setVec3("u_pointLight[1].ambient", pointLightColors[1].x * 0.1f, pointLightColors[1].y * 0.1f, pointLightColors[1].z * 0.1f);
		containerShader.setVec3("u_pointLight[1].diffuse", pointLightColors[1]);
		containerShader.setVec3("u_pointLight[1].specular", pointLightColors[1]);

		containerShader.setFloat("u_pointLight[1].constant", 1.0f);
		containerShader.setFloat("u_pointLight[1].linear", 0.09f);
		containerShader.setFloat("u_pointLight[1].quadratic", 0.032f);

		//POINT LIGHTING 3
		containerShader.setVec3("u_pointLight[2].position", pointLightPositions[2]);
		containerShader.setVec3("u_pointLight[2].ambient", pointLightColors[2].x * 0.1f, pointLightColors[2].y * 0.1f, pointLightColors[2].z * 0.1f);
		containerShader.setVec3("u_pointLight[2].diffuse", pointLightColors[2]);
		containerShader.setVec3("u_pointLight[2].specular", pointLightColors[2]);

		containerShader.setFloat("u_pointLight[2].constant", 1.0f);
		containerShader.setFloat("u_pointLight[2].linear", 0.09f);
		containerShader.setFloat("u_pointLight[2].quadratic", 0.032f);

		//POINT LIGHTING 4
		containerShader.setVec3("u_pointLight[3].position", pointLightPositions[3]);
		containerShader.setVec3("u_pointLight[3].ambient", pointLightColors[3].x * 0.1f, pointLightColors[3].y * 0.1f, pointLightColors[3].z * 0.1f);
		containerShader.setVec3("u_pointLight[3].diffuse", pointLightColors[3]);
		containerShader.setVec3("u_pointLight[3].specular", pointLightColors[3]);

		containerShader.setFloat("u_pointLight[3].constant", 1.0f);
		containerShader.setFloat("u_pointLight[3].linear", 0.09f);
		containerShader.setFloat("u_pointLight[3].quadratic", 0.032f);


		//SPOT LIGHTING
		containerShader.setVec3("u_spotLight.position", camera.position);
		containerShader.setVec3("u_spotLight.direction", camera.front);
		containerShader.setVec3("u_spotLight.ambient", 0.0f, 0.0f, 0.0f);
		containerShader.setVec3("u_spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		containerShader.setVec3("u_spotLight.specular", 1.0f, 1.0f, 1.0f);
		
		containerShader.setFloat("u_spotLight.constant", 1.0f);
		containerShader.setFloat("u_spotLight.linear", 0.22f);
		containerShader.setFloat("u_spotLight.quadratic", 0.20f);
		
		containerShader.setFloat("u_spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		containerShader.setFloat("u_spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// ---------- END OF LIGHTING ----------

		//rendering each cube
		glBindVertexArray(VAO[0]);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 cubeModel = glm::mat4(1.0f);
			cubeModel = glm::translate(cubeModel, cubePositions[i]);
			cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.51f, 0.0f));
			//enabling rotations
			float angle = 20.0f + (i * 3);
			cubeModel = glm::rotate(cubeModel, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			containerShader.setMat4("u_modelMatrix", cubeModel);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		//======= CURRENTLY NOT USELESS SINCE WE ARE USING THE LIGHT POSITION RN =======		
		//rendering light source
		glBindVertexArray(VAO[0]);
		lightCubeShader.useProgram();
		lightCubeShader.setMat4("u_projectionMatrix", projectionMatrix);
		lightCubeShader.setMat4("u_viewMatrix", cameraView);

		for (int i = 0; i < 4; i++) {
			glm::mat4 lightModel = glm::mat4(1.0f);
			lightCubeShader.setVec3("u_lightColor", pointLightColors[i]);
			lightModel = glm::translate(lightModel, pointLightPositions[i]);
			lightModel = glm::scale(lightModel, glm::vec3(0.5f));
			lightCubeShader.setMat4("u_modelMatrix", lightModel);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//rendering direction light source
		glBindVertexArray(VAO[0]);
		lightCubeShader.useProgram();
		lightCubeShader.setMat4("u_projectionMatrix", projectionMatrix);
		lightCubeShader.setMat4("u_viewMatrix", cameraView);
		glm::mat4 dirLightModel = glm::mat4(1.0f);
		lightCubeShader.setVec3("u_lightColor", glm::vec3(1.0f));
		dirLightModel = glm::translate(dirLightModel, lightDirection);
		lightCubeShader.setMat4("u_modelMatrix", dirLightModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//checking call events and swapping buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//optional: deleting the vertex arrays
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();		//clearing resources that were allocated
	return 0;
}

//ensuring the viewport gets resized if the user does so
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//function to handle user input
void processInput(GLFWwindow* window)
{
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
	if (ePressed && !s_eState)
	{
		s_fpsMode = !s_fpsMode;
		if (s_fpsMode)
		{
			std::cout << "FPS MODE ENABLED!" << std::endl;
			camera.position.y = 1.0f;
		}
		else
		{
			std::cout << "FREE FLY MODE ENABLED!" << std::endl;
		}
	}
	s_eState = ePressed;

	if (s_fpsMode)
	{
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
	else
	{
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
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);

	if (firstMouse)
	{
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
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.processMouseScroll(static_cast<float>(yOffset));
}

//function to handle loading textures using a path
unsigned int loadTexture(char const* path)
{
	unsigned int textureID{};
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	glGenTextures(1, &textureID);

	if (data)
	{
		GLenum textureFormat{};
		if (nrComponents == 1)
			textureFormat = GL_RED;
		if (nrComponents == 3)
			textureFormat = GL_RGB;
		if (nrComponents == 4)
			textureFormat = GL_RGBA;

		//binding the texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0,	textureFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//texture wrapping + mipmapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load textur at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}