#include "render_system.h"

#include <map>
#include <iomanip>
#include <sstream>

#include "renderer/render_context.h"
#include "renderer/texture_handler.h"

/*
//
// ========== DATA / VARIABLES ==========
// 


// Buffers 
unsigned int cubeVAO, cubeVBO;
unsigned int wallVAO, wallVBO, EBO;
unsigned int quadVAO, quadVBO;
unsigned int line_VAO, line_VBO;

unsigned int FBO, RBO;
unsigned int UBO_matrices;


// @TODO There's a lot of global state here with the shaders and the models, one of the suggestions was 
// create an AssetManager struct 
// Shaders
Shader containerShader;
Shader emissionShader;
Shader lightCubeShader;

Shader backpackShader;
Shader blahajShader;
Shader houseShader;
Shader explode_model_shader;

Shader floorShader;
Shader wallShader;
Shader line_shader;

Shader grassShader;
Shader windowShader;

Shader screenShader;
Shader font_shader;

// Models
Model backpack;
Model blahaj;
Model house;


// Textures
unsigned int diffuseMap  = 0;
unsigned int specularMap = 0;
unsigned int emissionMap = 0;

unsigned int floorTexture	  = 0;
unsigned int wallTexture 	  = 0;
unsigned int grassLandTexture = 0;
unsigned int grassTexture     = 0;
unsigned int windowTexture	  = 0;

unsigned int textureColorBuffer = 0;

// Matrixes - doing this instead of passing a camera reference to the draw calls because there isn't much else we want
// the camera for, so we just store the pos and front of the cam here
glm::mat4 projectionMatrix;

glm::mat4 cameraView;
glm::vec3 cameraPosition;
glm::vec3 cameraFront;


// Fonts
Font regular_text;
Font bold_text;
Font italic_text;
Font fps_text;


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


// FOR GEOMETRY SHADER TESTING:
float points[] = {
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
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

// @HARDCODE: Each shader that is affected by lighting properties
// has a hardcoded amount of point lights that matches the amount that
// is hardcoded here. Should be a better solution later
std::vector<glm::vec3> pointLightColors = {
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
void initBuffers() {
	const uint32_t width   = RenderState::SCREEN_WIDTH;
	const uint32_t height  = RenderState::SCREEN_HEIGHT;

	
	// Generating the relevant buffers
	// REMEMBER since some of these objects are not arrays, they need to be
	// referenced if they are a single obj
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1,		 &cubeVBO);
	glGenVertexArrays(1, &wallVAO);
	glGenBuffers(1,	     &wallVBO);
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1,		 &quadVBO);
	glGenVertexArrays(1, &line_VAO);
	glGenBuffers(1, 	 &line_VBO);

	glGenBuffers(1,		 &EBO);
	glGenFramebuffers(1,  &FBO);
	glGenTextures(1, 	  &textureColorBuffer);
	glGenRenderbuffers(1, &RBO);

	glGenBuffers(1, &UBO_matrices);


	// 3D Cubes (Positions + Normals + Textures)
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	
	// 2D Rendering walls / rectangles
	glBindVertexArray(wallVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// 2D line rendering
	glBindVertexArray(line_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, line_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// Currently not being used, but it's a good reminder as to how to
	// render a rectangle with the indices being utilised.
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndices), wallIndices, GL_STATIC_DRAW);

	// Binding a uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, UBO_matrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO_matrices, 0, 2 * sizeof(glm::mat4));


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


// @TODO: The specification of "nullptr" for these shaders that don't even need a
// geometry shader may need to get improved, therefore we should probably work on
// refactoring the whole init_shader() function by splitting the different shader
// objects that we construct.
void initShaders() {
	containerShader		= Shader("res/shaders/container.vert", "res/shaders/container.frag", nullptr);
	emissionShader		= Shader("res/shaders/container.vert", "res/shaders/emission.frag", nullptr);
	lightCubeShader		= Shader("res/shaders/container.vert", "res/shaders/lightCube.frag", nullptr);

	backpackShader	= Shader("res/shaders/backpack.vert", "res/shaders/backpack.frag", nullptr);
	blahajShader	= Shader("res/shaders/blahaj.vert", "res/shaders/blahaj.frag", nullptr);
	houseShader		= Shader("res/shaders/container.vert", "res/shaders/container.frag", nullptr);


	explode_model_shader = Shader("res/shaders/explode_model.vert", "res/shaders/explode_model.frag", "res/shaders/explode_model.geom");


	floorShader		= Shader("res/shaders/wall.vert", "res/shaders/wall.frag", nullptr);
	wallShader		= Shader("res/shaders/wall.vert", "res/shaders/wall.frag", nullptr);
	grassShader		= Shader("res/shaders/container.vert", "res/shaders/grass.frag", nullptr);
	windowShader	= Shader("res/shaders/container.vert", "res/shaders/window.frag", nullptr);

	screenShader	= Shader("res/shaders/screenbuffer.vert", "res/shaders/screenbuffer.frag", nullptr);
	font_shader		= Shader("res/shaders/font.vert", "res/shaders/font.frag", nullptr);
	
	line_shader		= Shader("res/shaders/geometry.vert", "res/shaders/geometry.frag", "res/shaders/geometry.geom");
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
		

void init_fonts() {
	regular_text = Font("res/fonts/Merriweather_24pt-Regular.ttf", 48);
	bold_text	 = Font("res/fonts/Merriweather_24pt-Bold.ttf", 48);
	italic_text	 = Font("res/fonts/Merriweather_24pt-Italic.ttf", 48);
	//fps_text	 = Font("res/fonts/Merriweather_24pt-Bold.ttf", 24);
}


//        
// ========== DRAWING OBJECTS ==========
//
void drawWoodenContainers() {
	bind_textures(containerShader, diffuseMap, specularMap, 0);
	containerShader.setFloat("u_material.shininess", 32.0f);
	applyMatrixes(containerShader);
	processLighting(containerShader);

	//drawing each cube (10 times)
	glBindVertexArray(cubeVAO);
	for (unsigned int i = 0; i < cubePositions.size(); i++) {
		glm::mat4 cubeModel = glm::mat4(1.0f);
		cubeModel = glm::translate(cubeModel, cubePositions[i]);
		cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.51f, 0.0f));
		//enabling rotations
		float angle = 20.0f + (i * 3);
		cubeModel = glm::rotate(cubeModel, Time::get_time() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		containerShader.setMat4("u_modelMatrix", cubeModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void drawEmissionContainer() {
	bind_textures(emissionShader, diffuseMap, specularMap, emissionMap);
	applyMatrixes(emissionShader);
	emissionShader.setFloat("u_material.shininess", 32.0f);
	processLighting(emissionShader);

	//drawing emission cube
	glBindVertexArray(cubeVAO);
	glm::mat4 emissionCubeModel = glm::mat4(1.0f);
	emissionCubeModel = glm::translate(emissionCubeModel, glm::vec3(5.0f, -2.0f, 7.0f));
	//enabling rotations
	float angle = 20.0f;
	emissionCubeModel = glm::rotate(emissionCubeModel, Time::get_time() *glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	emissionShader.setMat4("u_modelMatrix", emissionCubeModel);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawContainers() {
	drawWoodenContainers();
	drawEmissionContainer();
}


void drawPointLights() {
	glBindVertexArray(cubeVAO);
	lightCubeShader.useProgram();
	applyMatrixes(lightCubeShader);
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
	glBindVertexArray(cubeVAO);
	lightCubeShader.useProgram();
	applyMatrixes(lightCubeShader);
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
	backpackModel = glm::rotate(backpackModel, Time::get_time() * glm::radians(45.0f), glm::vec3(1.0f));
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
		blahajModel = glm::rotate(blahajModel, Time::get_time() * glm::radians(angle), glm::vec3(1.0f, 2.5f, 0.5f));
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


void draw_exploding_backpack() {
	explode_model_shader.useProgram();
	applyMatrixes(explode_model_shader);
	explode_model_shader.setFloat("u_material.shininess", 32.0f);
	explode_model_shader.setFloat("u_time", Time::get_time());
 	processLighting(explode_model_shader);

	glm::mat4 explode_model = glm::mat4(1.0f);
	explode_model = glm::translate(explode_model, glm::vec3(0.0f, -3.0f, 0.0f));
	explode_model = glm::scale(explode_model, glm::vec3(0.25f));
	explode_model = glm::rotate(explode_model, Time::get_time() * glm::radians(20.0f), glm::vec3(1.0f));
	explode_model_shader.setMat4("u_modelMatrix", explode_model);
	backpack.Draw(explode_model_shader);
}


void drawModels() {
	drawBackpack();
	drawBlahaj();
	drawHouse();

	draw_exploding_backpack();
}



void drawGrassLand() {
	bind_textures(floorShader, grassLandTexture, 0, 0);
	floorShader.setFloat("u_material.shininess", 1.0f);
	floorShader.setFloat("u_textureTiling", 64.0f);
	applyMatrixes(floorShader);
	processLighting(floorShader);

	glBindVertexArray(cubeVAO);
	glm::mat4 floorModel = glm::mat4(1.0f);
	floorModel = glm::translate(floorModel, glm::vec3(-10.0f, -5.0f, 5.0f));
	floorModel = glm::scale(floorModel, glm::vec3(100.0f, 0.001f, 100.0f));
	floorShader.setMat4("u_modelMatrix", floorModel);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawFoliage() {
	bind_textures(grassShader, grassTexture, 0, 0);
	grassShader.setFloat("u_material.shininess", 32.0f);
	applyMatrixes(grassShader);
	processLighting(grassShader);

	glBindVertexArray(wallVAO);
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
	bind_textures(windowShader, windowTexture, 0, 0);
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
	bind_textures(floorShader, floorTexture, 0, 0);
	floorShader.setFloat("u_material.shininess", 32.0f);
	floorShader.setFloat("u_textureTiling", 10.0f);
	applyMatrixes(floorShader);
	processLighting(floorShader);

	glBindVertexArray(cubeVAO);
	glm::mat4 floorModel = glm::mat4(1.0f);
	floorModel = glm::translate(floorModel, glm::vec3(-10.0f, -5.0f, 5.0f));
	floorModel = glm::scale(floorModel, glm::vec3(10.0f, 0.1f, 10.0f));
	floorShader.setMat4("u_modelMatrix", floorModel);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawWalls() {
	bind_textures(wallShader, wallTexture, 0, 0);
	wallShader.setFloat("u_material.shininess", 16.0f);
	wallShader.setFloat("u_textureTiling", 10.0f);
	applyMatrixes(wallShader);
	processLighting(wallShader);

	for (int i = 0; i < wallPositions.size(); i++) {
		glBindVertexArray(cubeVAO);
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


void draw_lines() {
	line_shader.useProgram();
	glBindVertexArray(line_VAO);
	glDrawArrays(GL_POINTS, 0, 4);
}


// Drawing relevant text information (like a HUD)
void display_fps() {
	static std::string string;
	static float current_time = 0.0f;
	static float time_acc	  = 0.0f;
	static uint32_t counter	  = 0;

	const glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
	const float alpha = 1.0f;
	const float scale = 0.5f;
	const TextAlign align = TextAlign::LEFT;
	const bool do_drop_shadow = true;

	const float x = 0;
	const float y = (float)RenderState::SCREEN_HEIGHT - 25;

	float delta_time = Time::get_delta_time();
	current_time = Time::get_time();
	counter++;

	time_acc += delta_time;
	if (time_acc >= 1.0f) {
		std::string fps = std::to_string(counter);
		std::string ms = std::to_string(1000.0f / (float)counter);

		// Displaying only fps for now
		// string = fps + "FPS / " + ms + "ms";
		string = fps + "FPS";

		counter = 0;
		time_acc = 0.0f;
	}
	
	bold_text.draw_text(font_shader, string, x, y, scale, color, alpha, align, do_drop_shadow); 
}


// Rendering world coordinates (with rounded floats to 2dp)
std::string format_coord(const std::string& label, float value){
	std::ostringstream string;
	string << label << ": " << std::fixed << std::setw(6) << std::setprecision(2) << value;
	return string.str();
}


void display_world_coords(Camera &camera) {
	static std::string x_str;
	static std::string y_str;
	static std::string z_str;

	const glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
	const float alpha = 1.0f;
	const float scale = 0.5f;
	const TextAlign align = TextAlign::LEFT;
	const bool do_drop_shadow = true;

	const float x = 0.0f;
	const float y = (float)RenderState::SCREEN_HEIGHT - 50;
//	const float y = 800.0f;

	x_str = format_coord("X", camera.position.x);
	y_str = format_coord("Y", camera.position.y);
	z_str = format_coord("Z", camera.position.z);


	bold_text.draw_text(font_shader, x_str, x, y, scale, color, alpha, align, do_drop_shadow);
	
	bold_text.draw_text(font_shader, y_str, x + 110, y, scale, color, alpha, align, do_drop_shadow);
	
	bold_text.draw_text(font_shader, z_str, x + 220, y, scale, color, alpha, align, do_drop_shadow);
}




//
// ========== RENDERING THE SCENE ==========
//
void render_scene(Camera& camera) {
	projectionMatrix = glm::perspective(glm::radians(camera.zoom), RenderState::ASPECT_RATIO, 0.1f, 100.0f);		//radians = FOV, width/height (aspect ratio), near and far plane	
	cameraView = camera.GetViewMatrix();
	cameraPosition = camera.position;
	cameraFront = camera.front;

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);

	skyColor = calculateSkyColor(Time::get_time());
	glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	// Setting the projection + view matrix into the uniform block
	glBindBuffer(GL_UNIFORM_BUFFER, UBO_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projectionMatrix));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cameraView));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	// Drawing stuff that is related to the skybox environment
	draw_skybox(projectionMatrix, camera);	
	draw_reflection_cube(cameraPosition);
	draw_refraction_cube(cameraPosition);

	// Drawing the floating stuff
	drawContainers();
	drawModels();
	drawLights();

	// Drawing everything else
	drawRoom();
	drawGrass();
	drawWindows(camera);	
//	draw_lines();

	// Using the screen shader for the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	screenShader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Anything that doesn't want to be post processed gets drawn here
	render_UI(camera);
}


void render_UI(Camera& camera){
	display_fps();	
	display_world_coords(camera);

	bold_text.update_and_draw_fading_texts(font_shader, Time::get_delta_time());
}

void cleanupScene() {
	delete_skybox_buffers();
	delete_vertex_data(cubeVAO, cubeVBO);	
	delete_vertex_data(wallVAO, wallVBO);
	delete_vertex_data(quadVAO, quadVBO);	

	glDeleteBuffers(1, 		 &EBO);
	glDeleteBuffers(1, 		 &UBO_matrices);	
	glDeleteFramebuffers(1,  &FBO);
	glDeleteRenderbuffers(1, &RBO);
}


//
// ========== UTILITY FUNCTIONS ==========
//
void applyMatrixes(Shader& shader) {
	shader.set_uniform_buffer("u_matrices", 0);
//	shader.setMat4("u_viewMatrix", cameraView);
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

		std::cout << "Texture loaded at path: " << path << "\n";
		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load texture at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// @HARDCODE: This whole lighting section is hardcoded based on the positions that were
// defined globally, along with the number of point light objects that are hardcoded within
// each shader.
void processLighting(Shader& shader) {
	//DIRECTIONAL LIGHTING
	shader.setVec3("u_dirLight.direction", lightDirection);
	shader.setVec3("u_dirLight.ambient", dirLightAmbient);
	shader.setVec3("u_dirLight.diffuse", dirLightDiffuse);
	shader.setVec3("u_dirLight.specular", dirLightSpecular);

	// POINT LIGHTING (count based on definition per shader)
	for (unsigned int i = 0; i < pointLightColors.size() ; i++) {
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

	// @TODO: This is probably not required in the near future because we are now
	// rendering a skybox that mamkes all the objects that fade into the fog kinda
	// awkward, therefore we may scrap this or rework the render distance.
	// FOG (USING SKY COLOR)
	shader.setFloat("u_fogDistance", 3.0f);
	shader.setVec3("u_skyColor", skyColor);
}


glm::vec3 calculateSkyColor(float current_t) {
	// Changing the color of the sky to simulate a day / night cycle
	float skyTransitionSpeed = 0.3f;
	float t = 0.5f * (1.0f + sin(skyTransitionSpeed * current_t));
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

void bind_textures(Shader& shader, unsigned int diffuse, unsigned int specular, unsigned int emission) {
	shader.useProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular);

	// Reuse the diffuse texture if the user has no specular texture being used
	if (specular == 0) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuse);
	}

	if (emission != 0) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emission);
	}
}

void delete_vertex_data (unsigned int& VAO, unsigned int& VBO) {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1,		&VBO);
}
*/




// @TODO: MASSIVE refactor of rendering things
bool init_camera(CameraData* camera_data, ViewportState* viewport) {
	camera_data->camera = create_camera(glm::vec3(0.0f, -4.0f, 10.0f));
	camera_data->aspect_ratio = viewport->aspect_ratio;

	camera_data->near_plane = 0.1f;
	camera_data->far_plane  = 100.0f;

	// Calculating initializing projection matrix
	camera_data->projection_matrix = glm::perspective(
		glm::radians(camera_data->camera.zoom),
		camera_data->aspect_ratio,
		camera_data->near_plane,
		camera_data->far_plane);

	std::cout << "----- Finished initializing camera! -----" << std::endl;
	return true;
}


bool init_buffers(BufferData* buffers, GeometryData* geometry, ViewportState* viewport, u32 texture_color_buffer) {
	const u32 width  = viewport->width;
	const u32 height = viewport->height;

	// Generating vertex arrays + buffers
	glGenVertexArrays(1, &buffers->cube_VAO);
	glGenBuffers(1,		 &buffers->cube_VBO);
	glGenVertexArrays(1, &buffers->wall_VAO);
	glGenBuffers(1,	     &buffers->wall_VBO);
	glGenVertexArrays(1, &buffers->line_VAO);
	glGenBuffers(1, 	 &buffers->line_VBO);
	glGenVertexArrays(1, &buffers->skybox_VAO);
	glGenBuffers(1,		 &buffers->skybox_VBO);
	glGenVertexArrays(1, &buffers->special_cube_VAO);
	glGenBuffers(1,		 &buffers->special_cube_VBO);

//	glGenBuffers(1,		  &buffers->EBO);					// Not needed right now.
	glGenVertexArrays(1,  &buffers->quad_VAO);
	glGenBuffers(1,		  &buffers->quad_VBO);
	glGenFramebuffers(1,  &buffers->FBO);
	glGenRenderbuffers(1, &buffers->RBO);

	glGenBuffers(1, &buffers->UBO_matrices);


	// 3D Cubes
	glBindVertexArray(buffers->cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->cube_vertices), geometry->cube_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// 2D Rendering walls / rectangles
	glBindVertexArray(buffers->wall_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->wall_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->wall_vertices), geometry->wall_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// 2D line rendering
	glBindVertexArray(buffers->line_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->line_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->points), geometry->points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// Skybox stuff
	glBindVertexArray(buffers->skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->skybox_vertices), geometry->skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	// Special cube (reflection + refractive cubes)
	glBindVertexArray(buffers->special_cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->special_cube_VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->cube_vertices_2), geometry->cube_vertices_2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// Currently not being used, but it's a good reminder as to how to
	// render a rectangle with the indices being utilised.
	/*
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndices), wallIndices, GL_STATIC_DRAW);
	*/


	// Frame buffer VAO + VBO
	glBindFramebuffer(GL_FRAMEBUFFER, buffers->FBO);
	glBindVertexArray(buffers->quad_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->quad_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->quad_vertices), geometry->quad_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	
	// Texture color attachment
	glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);


	// Setting render buffers
	glBindRenderbuffer(GL_RENDERBUFFER, buffers->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);		// Creating a depth + stencil render buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffers->RBO);
	// The rule with knowing when to use an RBO is when you never need to sample data from a buffer,
	// then you should use a render buffer for that specific buffer. BUT if you do need to sample data
	// (like color and texture values), then you should use a texture attachment instead.

	// Checking if the frame buffer status is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Binding a uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, buffers->UBO_matrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, buffers->UBO_matrices, 0, 2 * sizeof(glm::mat4));

	
	std::cout << "----- Finished initializing buffers! -----" << std::endl;
	return true;
}


// @Hardcode: similar to init_textures()
bool init_shaders(Assets* assets) {
	std::string base = "res/shaders/";

	struct ShaderPaths {
		const char* vertex;
		const char* fragment;
		const char* geometry;
	};
	
	// Must match the order seen in the ShaderType struct.
	const ShaderPaths shader_paths[] = {
		{"container.vert", "container.frag", nullptr},
		{"container.vert", "emission.frag", nullptr},
		{"container.vert", "light_cube.frag", nullptr},

		{"wall.vert", "wall.frag", nullptr},
		{"wall.vert", "wall.frag", nullptr},
		{"container.vert", "window.frag", nullptr},
		{"container.vert", "grass.frag", nullptr},

		{"backpack.vert", "backpack.frag", nullptr},
		{"blahaj.vert", "blahaj.frag", nullptr},
		{"container.vert", "container.frag", nullptr},
		{"explode_model.vert", "explode_model.frag", "explode_model.geom"},

		{"screenbuffer.vert", "screenbuffer.frag", nullptr},
		{"font.vert", "font.frag", nullptr},
		{"geometry.vert", "geometry.frag", "geometry.geom"},

		{"skybox.vert", "skybox.frag", nullptr},
		{"special_cube.vert", "reflection.frag", nullptr},
		{"special_cube.vert", "refraction.frag", nullptr},
	};

	// Creating each shader
	for (u32 i = 0; i < SHADER_COUNT; i++) {
		std::string vertex_path = base + shader_paths[i].vertex;
		std::string fragment_path = base + shader_paths[i].fragment;
		
		// If there is a geometry path given, then create the shader with it.
		std::string geometry_path = shader_paths[i].geometry ? (base + shader_paths[i].geometry) : "";

		assets->shaders[i] = create_shader(vertex_path.c_str(), fragment_path.c_str(), shader_paths[i].geometry ? geometry_path.c_str() : nullptr);


		// Error logging
		if (!assets->shaders[i].is_valid) {
			std::cout << "ERROR: Failed to create shader: " << i << std::endl;
			return false;
		}
	}

	std::cout << "----- Finished initializing shaders! -----" << std::endl;
	return true;
}

// @Hardcode: check note in render_data.h
bool init_models(Assets* assets) {
	std::string base = "res/models/";

	struct ModelPath {
		const char* path;
		bool flip_UVs;
	};

	const ModelPath model_paths[] = {
		{"backpack/backpack.obj", true},
		{"blahaj/blahaj.obj", false},
		{"house/house.fbx", true},
	};


	// Initializing each model
	for (u32 i = 0; i < MODEL_COUNT; i++) {
		assets->models[i] = create_model();

		if (model_paths[i].path != nullptr) {
			std::string full_path = base + model_paths[i].path;
			if (!load_model(&assets->models[i], full_path.c_str(), model_paths[i].flip_UVs, assets)) {
				std::cout << "ERROR: Failed to load model " << i << " at path: " << full_path << std::endl;
				return false;
			}
		}
	}

	std::cout << "----- Finished initializing models! -----" << std::endl;
	return true;
}

// @Hardcode: check note in render_data.h to see note regarding this function.
bool init_textures(Assets* assets) {
	std::string base = "res/textures/";

	// Array of texture paths (must match order of TextureType):
	const char* texture_2D_paths[] = {
		"container2.png",
		"container2_specular.png",
		"matrix.jpg",
		
		"dark_wooden_planks.jpg",
		"wallpaper.jpg",
		"transparent_window.png",
		"grass.png",
		"grassland.jpg",
		
		nullptr,
		nullptr
	};

	for (u32 i = 0; i < TEXTURE_COUNT; i++) {
		if (texture_2D_paths[i] != nullptr) {
			if(!add_2D_texture(assets, (TextureType)i, (base + texture_2D_paths[i]).c_str()))
				return false;
		}
	}


	// Loading cubemap textures
	std::vector<std::string> skybox_faces = {
		base + "skybox/right.jpg",
		base + "skybox/left.jpg",
		base + "skybox/top.jpg",
		base + "skybox/bottom.jpg",
		base + "skybox/front.jpg",
		base + "skybox/back.jpg",
	};
	if (!add_cubemap_texture(assets, TEXTURE_SKYBOX, skybox_faces))
		return false;


	// Loading texture color buffers - texture binding happens in init_buffers();
	glGenTextures(1, &assets->textures[TEXTURE_COLOR_BUFFER]);

	std::cout << "----- Finished initializing textures! -----" << std::endl;
	return true;
}


// @TODO: This could probably be improved, but we just want to get this working
// for the time being. We can do the same thing with using loops to match the
// FontType structure.
bool init_fonts(Assets* assets) {
//	std::string base = "res/fonts/";
	for (u32 i = 0; i < FONT_COUNT; i++) {
		assets->fonts[i] = create_font();
	}


	// Loading fonts that are in the scene's HUD 
	if (!load_font(&assets->fonts[FONT_REGULAR], "res/fonts/Merriweather_24pt-Regular.ttf", 42)) {
		return false;
	}

	if (!load_font(&assets->fonts[FONT_BOLD], "res/fonts/Merriweather_24pt-Bold.ttf", 42)) {
		return false;
	}

	if (!load_font(&assets->fonts[FONT_ITALIC], "res/fonts/Merriweather_24pt-Italic.ttf", 42)) {
		return false;
	}


	// Loading fonts that are seen in the menu
	if (!load_font(&assets->fonts[FONT_TITLE], "res/fonts/Merriweather_24pt-Italic.ttf", 96)) {
		return false;
	}
	
	if (!load_font(&assets->fonts[FONT_BODY], "res/fonts/Merriweather_24pt-Bold.ttf", 60)) {
		return false;
	}
	
	if (!load_font(&assets->fonts[FONT_SMALL], "res/fonts/Merriweather_24pt-Bold.ttf", 24)) {
		return false;
	}

	std::cout << "----- Finished initializing fonts! -----" << std::endl;
	return true;
}


// Managing the rendering system
bool init_rendering_system (RenderingContext* context) {
	std::cout << "Initializing rendering context..." << std::endl;

	// Error logging
	if (!init_camera(&context->camera_data, &context->viewport)) {
		std::cout << "ERROR: Failed to init_camera()" << std::endl;
		return false;
	}


	if (!init_textures(&context->assets)) {
		std::cout << "ERROR: Failed to init_textures()" << std::endl;
		return false;
	}
	
	if (!init_buffers(&context->buffers, &context->geometry, &context->viewport, context->assets.textures[TEXTURE_COLOR_BUFFER])) {
		std::cout << "ERROR: Failed to init_buffers()" << std::endl;
		return false;
	}

	if (!init_shaders(&context->assets)) {
		std::cout << "ERROR: Failed to init_shaders()" << std::endl;
		return false;
	}

	if (!init_models(&context->assets)) {
		std::cout << "ERROR: Failed to init_models()" << std::endl;
		return false;
	}

	if (!init_fonts(&context->assets)) {
		std::cout << "ERROR: Failed to init_fonts()" << std::endl;
		return false;
	}


	std::cout << "----- Finished initializing rendering context! -----" << std::endl;
	return true;
}


// @Incomplete: do - Cleanup functions
void cleanup_buffers(BufferData* buffers) {}
void cleanup_shaders(Assets* assets) {}
void cleanup_models(Assets* assets) {}
void cleanup_textures(Assets* assets) {}
void cleanup_fonts(Assets* assets) {}

bool cleanup_rendering_system (RenderingContext* context) {
	return true;
}



// Helper functions
void apply_matrices(const Shader* shader) {
	set_uniform_buffer(shader, "u_matrices", 0);
}

void process_lighting(const Shader* shader, const RenderingContext* context) {
	// Directional lighting
	set_vec3(shader, "u_dirLight.direction", context->lighting.directional_light_dir);
	set_vec3(shader, "u_dirLight.ambient", context->lighting.directional_ambient);
	set_vec3(shader, "u_dirLight.diffuse", context->lighting.directional_diffuse);
	set_vec3(shader, "u_dirLight.specular", context->lighting.directional_specular);

	// Point lighting
	for (u32 i = 0; i < (context->lighting.get_point_light_count()); i++) {
		std::string base = "u_pointLight[" + std::to_string(i) + "]";

		set_vec3(shader, (base + ".position").c_str(), context->lighting.point_light_positions[i]);
		set_vec3(shader, (base + ".ambient").c_str(), context->lighting.point_light_colors[i] * 0.1f);
		set_vec3(shader, (base + ".diffuse").c_str(), context->lighting.point_light_colors[i]);
		set_vec3(shader, (base + ".specular").c_str(), context->lighting.point_light_colors[i]);

		set_float(shader, (base + ".constant").c_str(), 1.0f);
		set_float(shader, (base + ".linear").c_str(), 0.09f);
		set_float(shader, (base + ".quadratic").c_str(), 0.032f);
	}

	// Spot lighting
	set_vec3(shader, "u_spotLight.position", context->camera_data.camera.position);
	set_vec3(shader, "u_spotLight.direction", context->camera_data.camera.front);
	set_vec3(shader, "u_spotLight.ambient", 0.0f, 0.0f, 0.0f);
	set_vec3(shader, "u_spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	set_vec3(shader, "u_spotLight.specular", 1.0f, 1.0f, 1.0f);

	set_float(shader, "u_spotLight.constant", 1.0f);
	set_float(shader, "u_spotLight.linear", 0.22f);
	set_float(shader, "u_spotLight.quadratic", 0.20f);

	set_float(shader, "u_spotLight.cutOff", glm::cos(glm::radians(10.0f)));
	set_float(shader, "u_spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

}

// Assumes that we are only binding one of each texture
void bind_textures(const Shader* shader, u32 diffuse, u32 specular, u32 emission) {
	use_shader(shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular);

	// Reuse the diffuse texture if the user has no specular texture being used
	if (specular == 0) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuse);
	}

	if (emission != 0) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emission);
	}
}


void set_texture_uniforms(const Shader* shader, bool do_emission) {
	use_shader(shader);
	set_int(shader, "u_material.textureDiffuse1", 0);
	set_int(shader, "u_material.textureSpecular1", 1);

	if (do_emission)
		set_int(shader, "u_material.textureEmission1", 2);
}


void update_camera_projection(RenderingContext* ctx) {
	CameraData* cd = &ctx->camera_data;
	ViewportState* viewport = &ctx->viewport;

	cd->projection_matrix = glm::perspective(
		glm::radians(cd->camera.zoom),
		viewport->aspect_ratio,
		cd->near_plane,
		cd->far_plane);

	// Updating the matrices block
	glm::mat4 view = get_view_matrix(&cd->camera);
	glBindBuffer(GL_UNIFORM_BUFFER, ctx->buffers.UBO_matrices);	
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cd->projection_matrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void resize_framebuffer(const RenderingContext* context, u32 width, u32 height) {
	glBindTexture(GL_TEXTURE_2D, context->assets.textures[TEXTURE_COLOR_BUFFER]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glBindRenderbuffer(GL_RENDERBUFFER, context->buffers.RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);	
}

void apply_render_mode_to_screen_shader(const RenderingContext* context) {
	const Shader* shader = &context->assets.shaders[SHADER_SCREEN];

	use_shader(shader);
	set_int(shader, "u_render_mode", static_cast<s32>(context->post_processing.mode));
}
