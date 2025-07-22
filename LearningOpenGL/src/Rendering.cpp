#include "Rendering.h"
#include "Time.h"
#include <map>

//
// ========== DATA / VARIABLES ==========
// 

// Buffers 
// @TODO Most likely going to change the VAOs and VBOs to be properly named to
// whatever vertex object they represent (VAO[0] = cubeVAO / VAO[1] = wanllVAO).
unsigned int VAO[2], VBO[2], EBO, FBO, RBO;
unsigned int quadVAO, quadVBO;


// @TODO There's a lot of global state here with the shaders and the models, one of the suggestions was 
// create an AssetManager struct 
// Shaders
Shader containerShader;
Shader emissionShader;
Shader lightCubeShader;

Shader backpackShader;
Shader blahajShader;
Shader houseShader;

Shader floorShader;
Shader wallShader;
Shader grassShader;
Shader windowShader;

Shader screenShader;


// Models
Model backpack;
Model blahaj;
Model house;


// Textures
// Currently unused (2D rendering objects use these ones)
/*
unsigned int texture1;
unsigned int texture2;
unsigned int texture3;
*/
unsigned int diffuseMap  = 0;
unsigned int specularMap = 0;
unsigned int emissionMap = 0;

unsigned int floorTexture = 0;
unsigned int wallTexture  = 0;

unsigned int grassLandTexture = 0;
unsigned int grassTexture     = 0;

unsigned int windowTexture = 0;

unsigned int textureColorBuffer = 0;

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

static float wallVertices[] = {
   -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

	0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
   -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
   -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
};

static float quadVertices[] = {  
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};	


// World positions of objects
std::vector<glm::vec3> cubePositions = {
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

std::vector<glm::vec3> pointLightPositions = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

std::vector<glm::vec3> blahajPositions = {
	glm::vec3(5.0f, 5.0f, -5.0f),
	glm::vec3(7.0f, 2.0f, 7.0f),
	glm::vec3(-6.0f, -1.0f, -5.0f),
	glm::vec3(4.0f, -3.0f, -1.0f),
	glm::vec3(5.0f, 0.0f, 5.0f),
};

std::vector<glm::vec3> wallPositions = {
	glm::vec3(-15.0f, -4.0f, 5.0f),  // left wall
	glm::vec3(-5.0f, -4.0f, 5.0f),	// right wall
	glm::vec3(-10.0f, -4.0f, 0.0f),	// front wall
	glm::vec3(-10.0f, -4.0f, 10.0f)	// back wall
};

std::vector<glm::vec3> foliagePositions = {
	glm::vec3(0.0f, -4.5f, 2.0f),
	glm::vec3(1.0f, -4.5f, 0.0f),
	glm::vec3(4.0f, -4.5f, 5.0f),
	glm::vec3(-2.0f, -4.5f, -1.0f),
	glm::vec3(-5.0f, -4.5f, -1.0f),
	glm::vec3(5.0f, -4.5f, 3.0f),
	glm::vec3(3.5f, -4.5f, -6.0f),
	glm::vec3(2.0f, -4.5f, -3.5f),
	glm::vec3(-2.0f, -4.5f, -7.0f),
	glm::vec3(7.0f, -4.5f, 2.0f),
};

float wallRotations[] = {
	0.0f,
	0.0f,
	90.0f,
	90.0f,
};

std::vector<glm::vec3> windowPositions = {
	glm::vec3(5.0f, -4.5f, 5.0f),
	glm::vec3(3.0f, -4.5f, 2.0f),
	glm::vec3(5.0f, -4.5f, 0.0f)
};


// Lighting properties
glm::vec3 lightDirection(1.2f, 3.0f, 2.0f);		// Directional lighting - can use vec4's w component to check if light is a position or direction (1.0f = position)
glm::vec3 dirLightAmbient(0.0f);
glm::vec3 dirLightDiffuse(0.05f);
glm::vec3 dirLightSpecular(0.2f);

std::vector<glm::vec3> pointLightColors = {					// Hardcoded the number of point lights here + in each shader
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
void initBuffers(const unsigned int width, const unsigned int height) {
	// Generating the relevant buffers
	// REMEMBER since some of these objects are not arrays, they need to be
	// referenced if they are a single obj
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glGenFramebuffers(1, &FBO);
	glGenTextures(1, &textureColorBuffer);
	glGenRenderbuffers(1, &RBO);
	

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

	
	// 2D Rendering walls / rectangles
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/*
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndices), wallIndices, GL_STATIC_DRAW);
	*/

	
	// Frame buffer VAO + VBO
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	// Setting texture color attachment
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	// Setting the render buffer attachments
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);		// Creating a depth + stencil render buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	// The rule with knowing when to use an RBO is when you never need to sample data from a buffer,
	// then you should use a render buffer for that specific buffer. BUT if you do need to sample data
	// (like color and texture values), then you should use a texture attachment instead.

	// Checking if the frame buffer status is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void initShaders() {
	containerShader	= Shader("res/shaders/container.vert", "res/shaders/container.frag");
	emissionShader	= Shader("res/shaders/container.vert", "res/shaders/emission.frag");
	lightCubeShader	= Shader("res/shaders/container.vert", "res/shaders/lightCube.frag");

	backpackShader	= Shader("res/shaders/backpack.vert", "res/shaders/backpack.frag");
	blahajShader	= Shader("res/shaders/blahaj.vert", "res/shaders/blahaj.frag");
	houseShader		= Shader("res/shaders/container.vert", "res/shaders/container.frag");

	floorShader		= Shader("res/shaders/wall.vert", "res/shaders/wall.frag");
	wallShader		= Shader("res/shaders/wall.vert", "res/shaders/wall.frag");
	grassShader		= Shader("res/shaders/container.vert", "res/shaders/grass.frag");
	windowShader	= Shader("res/shaders/container.vert", "res/shaders/window.frag");

	screenShader	= Shader("res/shaders/screenbuffer.vert", "res/shaders/screenbuffer.frag");
}


void initModels() {
	// 2nd param specifies whether to flip the texture UVs
	backpack	= Model("res/models/backpack/backpack.obj", true);
	blahaj		= Model("res/models/blahaj/blahaj.obj",		false);
	house		= Model("res/models/house/house.fbx",		true);
}


void initTextures() {
	// These maps are curently ONLY for the containers
	diffuseMap	= loadTexture("res/textures/container2.png");
	specularMap = loadTexture("res/textures/container2_specular.png");
	emissionMap = loadTexture("res/textures/matrix.jpg");

	floorTexture = loadTexture("res/textures/dark_wooden_planks.jpg");
	wallTexture	 = loadTexture("res/textures/wallpaper.jpg");

	grassTexture		= loadTexture("res/textures/grass.png");
	grassLandTexture	= loadTexture("res/textures/grassland.jpg");

	windowTexture		= loadTexture("res/textures/transparent_window.png");


	// Setting texture uniforms (that currently only have ONE of each
	// texture type, 2nd param specifies if there is an emission
	// texture that is to be included.
	set_texture_uniforms(containerShader,	false);
	set_texture_uniforms(wallShader,	    false);
	set_texture_uniforms(floorShader, 		false);
	set_texture_uniforms(windowShader,		false);
	set_texture_uniforms(emissionShader, 	true);


	// Screen frame buffer stuff
	screenShader.useProgram();
	screenShader.setInt("u_screenTexture", 0);
	screenShader.setInt("u_render_mode", 0);
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
	for (unsigned int i = 0; i < cubePositions.size(); i++) {
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

	for (int i = 0; i < pointLightPositions.size(); i++) {
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

	for (unsigned int i = 0; i < blahajPositions.size(); i++) {
		float angle = 20.0f * i;
		glm::mat4 blahajModel = glm::mat4(1.0f);
		blahajModel = glm::translate(blahajModel, blahajPositions[i]);
		blahajModel = glm::scale(blahajModel, glm::vec3(1.5f));
		blahajModel = glm::rotate(blahajModel, Time::getTime() * glm::radians(angle), glm::vec3(1.0f, 2.5f, 0.5f));
		blahajShader.setMat4("u_modelMatrix", blahajModel);
		blahaj.Draw(blahajShader);
	}
}


// @TODO The house vectors are successfully drawn at the right world pos, but the
// texturing of the model is currently messed up (possibly due to naming conventions
// or due to file formatting?
void drawHouse() {
	houseShader.useProgram();
	applyMatrixes(houseShader);
	houseShader.setFloat("u_material.shininess", 32.0f);
	processLighting(houseShader);

	glm::mat4 houseModel = glm::mat4(1.0f);
	houseModel = glm::translate(houseModel, glm::vec3(12.5f, -4.5f, 0.0f));
	houseModel = glm::scale(houseModel, glm::vec3(0.3f));
	houseModel = glm::rotate(houseModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	houseShader.setMat4("u_modelMatrix", houseModel);
	house.Draw(houseShader);
}

void drawModels() {
	drawBackpack();
	drawBlahaj();
	drawHouse();
}



void drawGrassLand() {
	//Drawing the grass land FLOOR
	floorShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassLandTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassLandTexture);
	floorShader.setFloat("u_material.shininess", 1.0f);
	floorShader.setFloat("u_textureTiling", 64.0f);
	applyMatrixes(floorShader);
	processLighting(floorShader);

	glBindVertexArray(VAO[0]);
	glm::mat4 floorModel = glm::mat4(1.0f);
	floorModel = glm::translate(floorModel, glm::vec3(-10.0f, -5.0f, 5.0f));
	floorModel = glm::scale(floorModel, glm::vec3(100.0f, 0.001f, 100.0f));
	floorShader.setMat4("u_modelMatrix", floorModel);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawFoliage() {
	grassShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTexture);	
	grassShader.setFloat("u_material.shininess", 32.0f);
	applyMatrixes(grassShader);
	processLighting(grassShader);

	glBindVertexArray(VAO[1]);
	for (unsigned int i = 0; i < foliagePositions.size(); i++) {
		glm::mat4 grassModel = glm::mat4(1.0f);
		grassModel = glm::translate(grassModel, foliagePositions[i]);
		grassModel = glm::rotate(grassModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		grassShader.setMat4("u_modelMatrix", grassModel);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void drawGrass() {
	drawGrassLand();
	drawFoliage();
}

void drawWindows(const Camera& camera) {
	windowShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, windowTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, windowTexture);
	windowShader.setFloat("u_material.shininess", 64.0f);
	applyMatrixes(windowShader);
	processLighting(windowShader);

	// Mapping the window positions based on which is the farthest away from
	// the camera. This allows us to draw the furthest ones away first to ensure
	// the windows will render properly behind eachother.
	std::map<float, glm::vec3> sortedWindows;
	for (unsigned int i = 0; i < windowPositions.size(); i++)
	{
		float distance = glm::length(camera.position - windowPositions[i]);
		sortedWindows[distance] = windowPositions[i];
	}

	//Drawing the windows at their positions based on the map
	for (std::map<float, glm::vec3>::reverse_iterator it = sortedWindows.rbegin(); it != sortedWindows.rend(); ++it)
	{
		glm::mat4 windowModel = glm::mat4(1.0f);
		windowModel = glm::translate(windowModel, it->second);
		windowModel = glm::rotate(windowModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		windowShader.setMat4("u_modelMatrix", windowModel);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
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
	floorModel = glm::translate(floorModel, glm::vec3(-10.0f, -5.0f, 5.0f));
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

	for (int i = 0; i < wallPositions.size(); i++) {
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

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);

	skyColor = calculateSkyColor(Time::getTime());
	glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	draw_skybox(projectionMatrix, camera);	

	//Drawing the floating stuff
	drawContainers();
	drawModels();
	drawLights();

	//Drawing everything else
	drawRoom();
	drawGrass();
	drawWindows(camera);	

	// Using the screen shader for the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	screenShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

}


void cleanupScene() {
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteFramebuffers(1, &FBO);
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
		if (textureFormat != GL_RGBA) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
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
	shader.setFloat("u_fogDistance", 3.0f);
	shader.setVec3("u_skyColor", skyColor);
}

glm::vec3 calculateSkyColor(float currentTime) {
	//changing the color of the sky to simulate a day / night cycle
	float skyTransitionSpeed = 0.3f;
	float t = 0.5f * (1.0f + sin(skyTransitionSpeed * currentTime));
	return skyColor = glm::mix(darkSky, greySky, t);
}

void resize_framebuffer(int width, int height) {
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);	
}


void apply_render_mode_to_screen_shader(RenderMode render_mode){
	screenShader.useProgram();
	screenShader.setInt("u_render_mode", static_cast<int> (render_mode));
}


// This is assuming that the naming convention we use for these
// uniforms are the exact same for each new shader that we create.
// This also only applies it to stuff that only requires EITHER a single
// diffuse or a diffuse + specular that exists within the textures
// folder (after we are done linking the textures)
void set_texture_uniforms(Shader& shader, bool do_emission) {
	shader.useProgram();
	shader.setInt("u_material.textureDiffuse1", 0);
	shader.setInt("u_material.textureSpecular1", 1);

	if (do_emission)
		shader.setInt("u_material.textureEmission1", 2);
}
