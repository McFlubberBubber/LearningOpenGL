#include "skybox.h"

/*
// @TODO: Needs to get refactored into the new refactor we doing

// Initializing the skybox's vertex data
void init_skybox() {
	glGenVertexArrays(1, &skybox_VAO);
	glGenBuffers(1, &skybox_VBO);

	glBindVertexArray(skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	
	// @TODO: Again, we are specifying nullptr here again
	//Initializing the skybox's textures
	skybox_shader   = Shader("res/shaders/skybox.vert", "res/shaders/skybox.frag", nullptr);
	cubemap_texture = load_cubemap(faces);
	skybox_shader.useProgram();
	skybox_shader.setInt("u_skybox", 0);
}


void init_reflection_cube() {
	glGenVertexArrays(1, &reflection_VAO);
	glGenBuffers(1,		 &reflection_VBO);

	glBindVertexArray(reflection_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, reflection_VAO);

		
	// 3D Cubes (Positions + Normals ONLY)
	glBufferData(GL_ARRAY_BUFFER, sizeof(reflection_cube_vertices), reflection_cube_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	reflection_shader = Shader("res/shaders/reflection.vert", "res/shaders/reflection.frag", nullptr);
	reflection_shader.useProgram();
	reflection_shader.setInt("u_skybox", 0);	
	reflection_shader.set_uniform_buffer("u_matrices", 0);
}

void init_refraction_cube() {
	glGenVertexArrays(1, &reflection_VAO);
	glGenBuffers(1,		 &reflection_VBO);

	glBindVertexArray(reflection_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, reflection_VAO);

		
	// 3D Cubes (Positions + Normals ONLY)
	glBufferData(GL_ARRAY_BUFFER, sizeof(reflection_cube_vertices), reflection_cube_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	refraction_shader = Shader("res/shaders/reflection.vert", "res/shaders/refraction.frag", nullptr);
	refraction_shader.useProgram();
	refraction_shader.setInt("u_skybox", 0);	
	refraction_shader.set_uniform_buffer("u_matrices", 0);
}


// This function will use different matrix stuff so we will bind the
// uniforms manually
void draw_skybox(const glm::mat4& projection_matrix, const Camera& camera) {
	// Ensuring the skybox will be centered around the player
	glm::mat4 view_matrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));

	// Depth test passes when values are equal to the depth buffer's
	// content
	glDepthMask(GL_FALSE);
//	glDepthFunc(GL_LEQUAL);
	skybox_shader.useProgram();

	skybox_shader.setMat4("u_projectionMatrix", projection_matrix);
	skybox_shader.setMat4("u_viewMatrix", view_matrix);

	glBindVertexArray(skybox_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);
	
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
//	glDepthFunc(GL_LESS);  
}

void draw_reflection_cube(const glm::vec3& view_position) {
	reflection_shader.useProgram();
	reflection_shader.setVec3("u_viewPosition", view_position);

	glBindVertexArray(reflection_VAO);
	glm::mat4 reflection_model = glm::mat4(1.0f);
	reflection_model = glm::translate(reflection_model, glm::vec3(0.0f, -4.0f, -7.0f));
	reflection_model = glm::rotate(reflection_model, Time::get_time() * glm::radians(80.0f), glm::vec3(1.0f, 0.5f, 2.5f));
	reflection_shader.setMat4("u_modelMatrix", reflection_model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void draw_refraction_cube(const glm::vec3& view_position) {
	refraction_shader.useProgram();
	refraction_shader.setVec3("u_viewPosition", view_position);

	glBindVertexArray(reflection_VAO);
	glm::mat4 refraction_model = glm::mat4(1.0f);
	refraction_model = glm::translate(refraction_model, glm::vec3(2.0f, -4.0f, -7.0f));
	refraction_model = glm::rotate(refraction_model, Time::get_time() * glm::radians(80.0f), glm::vec3(1.0f, 0.5f, 2.5f));
	refraction_shader.setMat4("u_modelMatrix", refraction_model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void delete_skybox_buffers () {
	glDeleteVertexArrays(1, &skybox_VAO);
	glDeleteBuffers(1,		&skybox_VBO);

	glDeleteVertexArrays(1, &reflection_VAO);
	glDeleteBuffers(1,		&reflection_VBO);
}
*/

