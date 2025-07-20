#include <iostream>

#include "Shader.h"
#include "Skybox.h"
#include "glad/glad.h"
#include "stb_image.h"

unsigned int skybox_VAO, skybox_VBO;
unsigned int cubemap_texture = 0;
Shader skybox_shader;

// Vertex data for the skybox (we don't need texture attributes)
static float skybox_vertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

// Texture handling
std::vector<std::string> faces = {
	"res/textures/skybox/right.jpg",
	"res/textures/skybox/left.jpg",
	"res/textures/skybox/top.jpg",
	"res/textures/skybox/bottom.jpg",
	"res/textures/skybox/front.jpg",
	"res/textures/skybox/back.jpg",
};


// Initializing the skybox's vertex data
void init_skybox() {
	glGenVertexArrays(1, &skybox_VAO);
	glGenBuffers(1, &skybox_VBO);

	glBindVertexArray(skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	//Initializing the skybox's textures
	skybox_shader   = Shader("res/shaders/skybox.vert", "res/shaders/skybox.frag");
	cubemap_texture = load_cubemap(faces);
	skybox_shader.setInt("u_skybox", 0);
}


// @HARDCODE: Since we are loading just the one cubemap that is
// dedicated to the skybox, we will disable the flip_vertically flag
// manually for the time being. If we ever are going to refactor this
// code to allow for more cube maps to load, we can add a flag ourselves
// so that this function can be reused.
// Loading all the 6 texture faces and binding them to the texture_id
unsigned int load_cubemap(std::vector<std::string> faces) {
	stbi_set_flip_vertically_on_load(false);

	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	int width, height, nr_channels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nr_channels, 0);

		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		} else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	stbi_set_flip_vertically_on_load(true);	
	return texture_id;
}


// This function will use different matrix stuff so we will bind the
// uniforms manually
void draw_skybox(const glm::mat4& projection_matrix, const Camera& camera) {
	// Ensuring the skybox will be centered around the player
	glm::mat4 view_matrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));

	// Depth test passes when values are equal to the depth buffer's
	// content
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	skybox_shader.useProgram();

	skybox_shader.setMat4("u_projectionMatrix", projection_matrix);
	skybox_shader.setMat4("u_viewMatrix", view_matrix);

	glBindVertexArray(skybox_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
	
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);  
}
