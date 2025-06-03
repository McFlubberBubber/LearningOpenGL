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
bool useTexture = true;		//whether to use textures on a shader / use normals

//global variable that positions the light - can use vec4's w component to check if light is a position or direction (1.0f = position)
glm::vec3 lightPosition(0.0f, 5.0f, 0.0f);
glm::vec3 lightDirection(0.0f, -5.0f, 0.0f);

//setting up camera
Camera camera(glm::vec3(0.0f, 1.0f, 0.0f));
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

	/*
	//setting the pos, normal and texture attributes of the brick wall (2D)
	float wallVertices[] = {
		// positions           // normals           // texture coords
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     0.0f, 1.0f    // top left
	};


	//the numbering is like this because it begins drawing anticlockwise
	unsigned int wallIndices[] = {
		0, 1, 3,		//first triangle
		1, 2, 3			//second traingle
	};

	*/

	//creating the WORLD positions for the wall
	glm::vec3 wallPositions[] = {
		glm::vec3(0.0f,  1.0f,  5.0f),		//front wall
		glm::vec3(5.0f,  1.0f,  0.0f),		//right wall
		glm::vec3(0.0f,  1.0f,  -5.0f),		//back wall
		glm::vec3(-5.0f,  1.0f,  0.0f),		//left wall
	};


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
		glm::vec3(1.0f,  0.0f,  3.0f),
		glm::vec3(-3.0f,  0.0f,  2.0f),
		glm::vec3(2.0f,  0.0f, -3.0f),
		glm::vec3(4.0f,  0.0f,  -1.0f),
		glm::vec3(-3.0f,  0.0f,  -3.0f),
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

	/* 2D RENDERING STUFF
	wallShader.useProgram();
	wallShader.setInt("material.diffuse", 0);
	*/

	lightingShader.useProgram();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	lightingShader.setInt("material.emission", 2);

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
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//clearing the buffers every iteration

		//creating matrixes
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.zoom), ASPECT_RATIO, 0.1f, 100.0f);		//radians = FOV, width/height (aspect ratio), near place and far plane	
		glm::mat4 cameraView = camera.GetViewMatrix();
		
		
		//drawing some 3D walls
		glBindVertexArray(VAO[0]);
		wallShader.useProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		//setting material properties
		wallShader.setVec3("material.specular", glm::vec3(0.5f));
		wallShader.setFloat("material.shininess", 32.0f);
		//setting light properties
		//wallShader.setVec3("light.position", lightPosition);
		wallShader.setVec3("light.direction", lightDirection);
		wallShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		wallShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
		wallShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		//setting uniforms
		wallShader.setMat4("u_projectionMatrix", projectionMatrix);
		wallShader.setMat4("u_viewMatrix", cameraView);
		wallShader.setVec3("u_viewPosition", camera.position);

		for (int i = 0; i < 4; i++)
		{
			glm::mat4 wallModel3D = glm::mat4(1.0f);
			wallModel3D = glm::translate(wallModel3D, wallPositions[i]);

			//rotating the walls based on the index
			if (i == 1)
				wallModel3D = glm::rotate(wallModel3D, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			if (i == 2)
				wallModel3D = glm::rotate(wallModel3D, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			if (i == 3)
				wallModel3D = glm::rotate(wallModel3D, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			wallModel3D = glm::scale(wallModel3D, glm::vec3(9.75f, 2.5f, 0.25f));
			wallModel3D = glm::translate(wallModel3D, glm::vec3(0.0f, 0.1f, 0.0f));

			wallShader.setMat4("u_modelMatrix", wallModel3D);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//rendering the floor
		glBindVertexArray(VAO[0]);
		wallShader.useProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		//setting material properties
		wallShader.setVec3("material.specular", glm::vec3(0.5f));
		wallShader.setFloat("material.shininess", 32.0f);
		//setting light properties
		//wallShader.setVec3("light.position", lightPosition);
		wallShader.setVec3("light.direction", lightDirection);
		wallShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		wallShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
		wallShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		//setting uniforms
		wallShader.setMat4("u_projectionMatrix", projectionMatrix);
		wallShader.setMat4("u_viewMatrix", cameraView);
		wallShader.setVec3("u_viewPosition", camera.position);

		glm::mat4 floorModel = glm::mat4(1.0f);
		floorModel = glm::rotate(floorModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		floorModel = glm::scale(floorModel, glm::vec3(9.5f, 9.5f, 0.01f));
		wallShader.setMat4("u_modelMatrix", floorModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//rendering 3D cubes
		containerShader.useProgram();
		//textures for containers
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		//setting material properties
		containerShader.setFloat("material.shininess", 32.0f);
		//setting light properties
		//containerShader.setVec3("light.position", lightPosition);
		containerShader.setVec3("light.direction", lightDirection);
		containerShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		containerShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
		containerShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		//setting uniforms
		containerShader.setVec3("u_viewPosition", camera.position);
		containerShader.setMat4("u_projectionMatrix", projectionMatrix); //applying matrixes
		containerShader.setMat4("u_viewMatrix", cameraView);

		//rendering each cube
		glBindVertexArray(VAO[0]);
		for (unsigned int i = 0; i < 5; i++)
		{
			glm::mat4 cubeModel = glm::mat4(1.0f);
			cubeModel = glm::translate(cubeModel, cubePositions[i]);
			cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.51f, 0.0f));

			//enabling rotations
			//float angle = 60.0f;
			//cubeModel = glm::rotate(cubeModel, (float)glfwGetTime() * glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

			containerShader.setMat4("u_modelMatrix", cubeModel);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		
		//rendering the light affected cube
		glBindVertexArray(VAO[0]);
		lightingShader.useProgram();
		//binding texture maps
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emissionMap);
		//setting material properties
		lightingShader.setFloat("material.shininess", 32.0f);
		//setting light properties
		//lightingShader.setVec3("light.position", lightPosition);				//positional lighting
		lightingShader.setVec3("light.direction", lightDirection);				//directional lighting
		lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		//setting uniforms
		lightingShader.setMat4("u_projectionMatrix", projectionMatrix);
		lightingShader.setMat4("u_viewMatrix", cameraView);
		lightingShader.setVec3("u_viewPosition", camera.position);

		//drawing the models
		glm::mat4 lightModel = glm::mat4(1.0f);	
		lightModel = glm::translate(lightModel, glm::vec3(0.0f, 3.5f, 0.0f));
		lightModel = glm::scale(lightModel, glm::vec3(1.0f, 1.0f, 1.0f));
		//lightModel = glm::rotate(lightModel, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setMat4("u_modelMatrix", lightModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		/*
		//======= CURRENTLY USELESS SINCE WE ARE USING THE LIGHT DIRECTION INSTEAD =======		
		//rendering light source
		glBindVertexArray(VAO[0]);
		lightCubeShader.useProgram();
		lightCubeShader.setMat4("u_projectionMatrix", projectionMatrix);
		lightCubeShader.setMat4("u_viewMatrix", cameraView);

		lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, lightPosition);
		lightModel = glm::scale(lightModel, glm::vec3(0.5f));
		lightCubeShader.setMat4("u_modelMatrix", lightModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// change the light's position values over time (can be done anywhere in the render loop actually, but try to do it at least before using the light source positions)
		lightPosition.x = sin(glfwGetTime()) * 3.0f;
		lightPosition.z = cos(glfwGetTime()) * 3.0f;
		lightPosition.y = 3.5f;
		*/

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