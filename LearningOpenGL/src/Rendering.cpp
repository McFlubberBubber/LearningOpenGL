#include "Rendering.h"
#include "Time.h"

//
// ========== DATA / VARIABLES ==========
// 

// Buffers 
unsigned int VAO[2], VBO[2], EBO;		//these are currently set to 2, one for 3D cubes, and the other for 2D that we don't use

// @TODO There's a lot of global state here with the shaders and the models, one of the suggestions was 
// create an AssetManager struct 
// Shaders
Shader containerShader;
Shader emissionShader;
Shader lightCubeShader;
Shader backpackShader;
Shader blahajShader;
Shader floorShader;
Shader wallShader;
Shader grassShader;

// Models
Model backpack;
Model blahaj;

// Textures
// Currently unused (2D rendering objects use these ones)
/*
unsigned int texture1;
unsigned int texture2;
unsigned int texture3;
*/
unsigned int diffuseMap = 0;
unsigned int specularMap = 0;
unsigned int emissionMap = 0;

unsigned int grassTexture = 0;
unsigned int floorTexture = 0;
unsigned int wallTexture = 0;


// Matrixes - doing this instead of passing a camera reference to the draw calls because there isn't much else we want
// the camera for, so we just store the pos and front of the cam here
glm::mat4 projectionMatrix;
glm::mat4 cameraView;
glm::vec3 cameraPosition;
glm::vec3 cameraFront;


// Consists of position, normals and textures
static float cubeVertices[] = {
	// Back face
	-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	0.5f,  0.5f, -0.5f,	   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,	   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,	   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	// Front face
	-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
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
	0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,

	// Top face
	-0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,    0.0f,  1.0f, 0.0f,   1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,    0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,    0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f
};

// World positions of objects
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

glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

glm::vec3 blahajPositions[] = {
	glm::vec3(5.0f, 5.0f, -5.0f),
	glm::vec3(7.0f, 2.0f, 7.0f),
	glm::vec3(-6.0f, -1.0f, -5.0f),
	glm::vec3(4.0f, -3.0f, -1.0f),
	glm::vec3(5.0f, 0.0f, 5.0f),
};

glm::vec3 wallPositions[] = {
	glm::vec3(-15.0f, 1.0f, 5.0f),  // left wall
	glm::vec3(-5.0f, 1.0f, 5.0f),	// right wall
	glm::vec3(-10.0f, 1.0f, 0.0f),	// front wall
	glm::vec3(-10.0f, 1.0f, 10.0f)	// back wall
};

float wallRotations[] = {
	0.0f,
	0.0f,
	90.0f,
	90.0f,
};

// Lighting properties
glm::vec3 lightDirection(1.2f, 3.0f, 2.0f);		// Directional lighting - can use vec4's w component to check if light is a position or direction (1.0f = position)
glm::vec3 dirLightAmbient(0.0f);
glm::vec3 dirLightDiffuse(0.05f);
glm::vec3 dirLightSpecular(0.2f);

glm::vec3 pointLightColors[]{					// Hardcoded the number of point lights here + in each shader
	glm::vec3(0.75f, 0.75f, 0.75f),
	glm::vec3(0.75f, 0.0f, 0.60f),
	glm::vec3(0.0f, 0.0f, 0.8f),
	glm::vec3(0.75f, 0.05f, 0.05f)
};

//Sky coloring
glm::vec3 darkSky(0.001f, 0.001f, 0.001f);
glm::vec3 greySky(0.5f, 0.5f, 0.5f);
glm::vec3 skyColor;



//
// ========== INITIALIZATION ==========
//
void initBuffers() {							// Size of the VAOs and VBOs are subject to change
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	// 3D Rendering cubes
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
	// 2D Rendering walls / rectangles
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
}


void initShaders() {
	containerShader	= Shader("res/shaders/container.vert", "res/shaders/container.frag");
	emissionShader	= Shader("res/shaders/container.vert", "res/shaders/emission.frag");
	lightCubeShader	= Shader("res/shaders/container.vert", "res/shaders/lightCube.frag");
	backpackShader	= Shader("res/shaders/backpack.vert", "res/shaders/backpack.frag");
	blahajShader	= Shader("res/shaders/blahaj.vert", "res/shaders/blahaj.frag");
	floorShader		= Shader("res/shaders/wall.vert", "res/shaders/wall.frag");
	wallShader		= Shader("res/shaders/wall.vert", "res/shaders/wall.frag");

}


void initModels() {
	// 2nd param specifies whether to flip the texture UVs
	backpack	= Model("res/models/backpack/backpack.obj", false);
	blahaj		= Model("res/models/blahaj/blahaj.obj", true);
}


void initTextures() {
	// @TODO Naming could be better here?
	// but also, these textures aren't even being used, so they are commented out for now
	/*
	texture1 = loadTexture("res/textures/container.jpg");
	texture2 = loadTexture("res/textures/awesomeface.png");
	texture3 = loadTexture("res/textures/wall.jpg");
	*/

	// These maps are curently ONLY for the containers
	diffuseMap	= loadTexture("res/textures/container2.png");
	specularMap = loadTexture("res/textures/container2_specular.png");
	emissionMap = loadTexture("res/textures/matrix.jpg");

	// Textures for the room scene
	floorTexture = loadTexture("res/textures/dark_wooden_planks.jpg");
	wallTexture	 = loadTexture("res/textures/wallpaper.jpg");


	// @TODO These textures could possibly be called in a seperate function so clean
	// up this code since the shaders are using the same integers and texture uniforms
	//setting texture uniforms
	containerShader.useProgram();
	containerShader.setInt("u_material.textureDiffuse1", 0);
	containerShader.setInt("u_material.textureSpecular1", 1);

	wallShader.useProgram();
	wallShader.setInt("u_material.textureDiffuse1", 0);
	wallShader.setInt("u_material.textureSpecular1", 1);

	floorShader.useProgram();
	floorShader.setInt("u_material.textureDiffuse1", 0);
	floorShader.setInt("u_material.textureSpecular1", 1);

	// This one is different because it uses an extra emission integer
	emissionShader.useProgram();
	emissionShader.setInt("u_material.textureDiffuse1", 0);
	emissionShader.setInt("u_material.textureSpecular1", 1);
	emissionShader.setInt("u_material.textureEmission1", 2);


}
		


//
// ========== DRAWING OBJECTS ==========
//
void drawWoodenContainers() {
	containerShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	containerShader.setFloat("u_material.shininess", 32.0f);
	applyMatrixes(containerShader);
	processLighting(containerShader);

	//drawing each cube (10 times)
	glBindVertexArray(VAO[0]);
	for (unsigned int i = 0; i < 10; i++) {
		glm::mat4 cubeModel = glm::mat4(1.0f);
		cubeModel = glm::translate(cubeModel, cubePositions[i]);
		cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.51f, 0.0f));
		//enabling rotations
		float angle = 20.0f + (i * 3);
		cubeModel = glm::rotate(cubeModel, Time::getTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		containerShader.setMat4("u_modelMatrix", cubeModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void drawEmissionContainer() {
	emissionShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, emissionMap);
	applyMatrixes(emissionShader);
	emissionShader.setFloat("u_material.shininess", 32.0f);
	processLighting(emissionShader);

	//drawing emission cube
	glBindVertexArray(VAO[0]);
	glm::mat4 emissionCubeModel = glm::mat4(1.0f);
	emissionCubeModel = glm::translate(emissionCubeModel, glm::vec3(5.0f, -2.0f, 7.0f));
	//enabling rotations
	float angle = 20.0f;
	emissionCubeModel = glm::rotate(emissionCubeModel, Time::getTime() *glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	emissionShader.setMat4("u_modelMatrix", emissionCubeModel);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawContainers() {
	drawWoodenContainers();
	drawEmissionContainer();
}


void drawPointLights() {
	glBindVertexArray(VAO[0]);
	lightCubeShader.useProgram();
	applyMatrixes(lightCubeShader);					// @TODO possible break since there is no u_viewPosition in the frag shader
	lightCubeShader.setVec3("u_skyColor", skyColor);

	for (int i = 0; i < 4; i++) {
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightCubeShader.setVec3("u_lightColor", pointLightColors[i]);
		lightModel = glm::translate(lightModel, pointLightPositions[i]);
		lightModel = glm::scale(lightModel, glm::vec3(0.5f));
		lightCubeShader.setMat4("u_modelMatrix", lightModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void drawDirectionalLight() {
	glBindVertexArray(VAO[0]);
	lightCubeShader.useProgram();
	applyMatrixes(lightCubeShader);				// same here
	glm::mat4 dirLightModel = glm::mat4(1.0f);
	lightCubeShader.setVec3("u_lightColor", glm::vec3(1.0f));
	dirLightModel = glm::translate(dirLightModel, lightDirection);
	lightCubeShader.setMat4("u_modelMatrix", dirLightModel);
	lightCubeShader.setVec3("u_skyColor", skyColor);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawLights() {
	drawPointLights();
	drawDirectionalLight();
}


void drawBackpack() {
	backpackShader.useProgram();
	applyMatrixes(backpackShader);
	backpackShader.setFloat("u_material.shininess", 32.0f);
	processLighting(backpackShader);

	glm::mat4 backpackModel = glm::mat4(1.0f);
	backpackModel = glm::translate(backpackModel, glm::vec3(0.0f, 0.0f, -6.0f));
	backpackModel = glm::scale(backpackModel, glm::vec3(0.5f));
	backpackModel = glm::rotate(backpackModel, Time::getTime() * glm::radians(45.0f), glm::vec3(1.0f));
	backpackShader.setMat4("u_modelMatrix", backpackModel);
	backpack.Draw(backpackShader);
}

void drawBlahaj() {
	blahajShader.useProgram();
	applyMatrixes(blahajShader);
	blahajShader.setFloat("u_material.shininess", 32.0f);
	processLighting(blahajShader);

	for (unsigned int i = 0; i < 5; i++) {
		float angle = 20.0f * i;
		glm::mat4 blahajModel = glm::mat4(1.0f);
		blahajModel = glm::translate(blahajModel, blahajPositions[i]);
		blahajModel = glm::scale(blahajModel, glm::vec3(1.5f));
		blahajModel = glm::rotate(blahajModel, Time::getTime() * glm::radians(angle), glm::vec3(1.0f, 2.5f, 0.5f));
		blahajShader.setMat4("u_modelMatrix", blahajModel);
		blahaj.Draw(blahajShader);
	}
}

void drawModels() {
	drawBackpack();
	drawBlahaj();
}


// Probably not gonna be used
void drawGrass() {
	std::cout << "Should be drawing grass rn" << std::endl;
}

void drawFloor() {
	floorShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	floorShader.setFloat("u_material.shininess", 32.0f);
	floorShader.setFloat("u_textureTiling", 10.0f);
	applyMatrixes(floorShader);
	processLighting(floorShader);

	glBindVertexArray(VAO[0]);
	glm::mat4 floorModel = glm::mat4(1.0f);
	floorModel = glm::translate(floorModel, glm::vec3(-10.0f, 0.0f, 5.0f));
	floorModel = glm::scale(floorModel, glm::vec3(10.0f, 0.1f, 10.0f));
	floorShader.setMat4("u_modelMatrix", floorModel);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawWalls() {
	wallShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wallTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, wallTexture);
	wallShader.setFloat("u_material.shininess", 16.0f);
	wallShader.setFloat("u_textureTiling", 10.0f);
	applyMatrixes(wallShader);
	processLighting(wallShader);

	for (int i = 0; i < 4; i++) {
		glBindVertexArray(VAO[0]);
		glm::mat4 wallModel = glm::mat4(1.0f);
		wallModel = glm::translate(wallModel, wallPositions[i]);
		wallModel = glm::rotate(wallModel, glm::radians(wallRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f));
		wallModel = glm::scale(wallModel, glm::vec3(0.1f, 2.0f, 10.0f)); // thickness, height, length
		wallShader.setMat4("u_modelMatrix", wallModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void drawRoom() {
	drawFloor();
	drawWalls();
}

//
// ========== RENDERING THE SCENE ==========
//
void renderScene(Camera& camera, const float ASPECT_RATIO) {
	projectionMatrix = glm::perspective(glm::radians(camera.zoom), ASPECT_RATIO, 0.1f, 100.0f);		//radians = FOV, width/height (aspect ratio), near and far plane	
	cameraView = camera.GetViewMatrix();
	cameraPosition = camera.position;
	cameraFront = camera.front;

	skyColor = calculateSkyColor(Time::getTime());
	glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Drawing the floating stuff
	drawContainers();
	drawModels();
	drawLights();

	void drawGrass();

	//Drawing the room
	drawRoom();
}

void cleanupScene() {
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, &EBO);
}

//
// ========== UTILITY FUNCTIONS ==========
//
void applyMatrixes(Shader& shader) {
	shader.setVec3("u_viewPosition", cameraPosition);
	shader.setMat4("u_projectionMatrix", projectionMatrix);
	shader.setMat4("u_viewMatrix", cameraView);
}



unsigned int loadTexture(const char* path) {
	unsigned int textureID{};
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	glGenTextures(1, &textureID);

	if (data) {
		GLenum textureFormat{};
		if (nrComponents == 1)
			textureFormat = GL_RED;
		if (nrComponents == 3)
			textureFormat = GL_RGB;
		if (nrComponents == 4)
			textureFormat = GL_RGBA;

		//binding the texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, textureFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//texture wrapping + mipmapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load texture at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


void processLighting(Shader& shader) {
	//DIRECTIONAL LIGHTING
	shader.setVec3("u_dirLight.direction", lightDirection);
	shader.setVec3("u_dirLight.ambient", dirLightAmbient);
	shader.setVec3("u_dirLight.diffuse", dirLightDiffuse);
	shader.setVec3("u_dirLight.specular", dirLightSpecular);

	//POINT LIGHTING (count based on definition per shader)
	for (unsigned int i = 0; i < 4; i++) {
		//converting i to a string to utilise within uniform setting
		std::string index = std::to_string(i);

		shader.setVec3("u_pointLight[" + index + "].position", pointLightPositions[i]);
		shader.setVec3("u_pointLight[" + index + "].ambient", pointLightColors[i] * 0.1f);
		shader.setVec3("u_pointLight[" + index + "].diffuse", pointLightColors[i]);
		shader.setVec3("u_pointLight[" + index + "].specular", pointLightColors[i]);

		shader.setFloat("u_pointLight[" + index + "].constant", 1.0f);
		shader.setFloat("u_pointLight[" + index + "].linear", 0.09f);
		shader.setFloat("u_pointLight[" + index + "].quadratic", 0.032f);
	}

	//SPOT LIGHTING
	shader.setVec3("u_spotLight.position", cameraPosition);
	shader.setVec3("u_spotLight.direction", cameraFront);
	shader.setVec3("u_spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shader.setVec3("u_spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("u_spotLight.specular", 1.0f, 1.0f, 1.0f);

	shader.setFloat("u_spotLight.constant", 1.0f);
	shader.setFloat("u_spotLight.linear", 0.22f);
	shader.setFloat("u_spotLight.quadratic", 0.20f);

	shader.setFloat("u_spotLight.cutOff", glm::cos(glm::radians(10.0f)));
	shader.setFloat("u_spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	//FOG (USING SKY COLOR)
	shader.setVec3("u_skyColor", skyColor);
}

glm::vec3 calculateSkyColor(float currentTime) {
	//changing the color of the sky to simulate a day / night cycle
	float skyTransitionSpeed = 1.0f;
	float t = 0.5f * (1.0f + sin(skyTransitionSpeed * currentTime));
	return skyColor = glm::mix(darkSky, greySky, t);
}