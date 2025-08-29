#include "mesh.h"

#include "renderer/shader.h"	// For using uniform functions

Mesh create_mesh(std::vector<Vertex> vertices, std::vector<u32> indices, std::vector<MeshTexture> textures) {
	Mesh mesh = {};
	mesh.vertices = std::move(vertices);
	mesh.indices  = std::move(indices);
	mesh.textures = std::move(textures);
	mesh.is_setup = false;
	return mesh;
}


void setup_mesh(Mesh* mesh) {
	// Generating vertex arrays + buffers
	glGenVertexArrays(1, &mesh->VAO);
	glGenBuffers(1,		 &mesh->VBO);
	glGenBuffers(1,		 &mesh->EBO);

	// Binding arrays
	glBindVertexArray(mesh->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(u32), &mesh->indices[0], GL_STATIC_DRAW);


	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// Textures
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));

	glBindVertexArray(0);
	mesh->is_setup = true;
}


void destroy_mesh(Mesh* mesh) {
	if(mesh->is_setup) {
		glDeleteVertexArrays(1, &mesh->VAO);
		glDeleteBuffers(1,		&mesh->VBO);
		glDeleteBuffers(1,		&mesh->EBO);
		mesh->is_setup = false;
	}
}


void draw_mesh(const Mesh* mesh, const Shader* shader) {
	if (!mesh->is_setup) return;

	u32 diffuse_nr = 1;
	u32 specular_nr = 1;
	u32 emission_nr = 1;

	for (u32 i = 0; i < mesh->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = mesh->textures[i].type;

		// Determining texture number based on the type
		if (name == "textureDiffuse")
			number = std::to_string(diffuse_nr++);
		if (name == "textureSpecular")
			number = std::to_string(specular_nr++);
		if (name == "textureEmission")
			number = std::to_string(emission_nr++);
		
		// @TODO: The model shaders may need their naming conventions updated.
		set_int(shader, ("u_material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);


	// Drawing the mesh
	glBindVertexArray(mesh->VAO);
	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
