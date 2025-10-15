#include "mesh.h"
#include "renderer/shader.h"

Mesh create_mesh(std::vector<Vertex> vertices, std::vector<u32> indices, std::vector<MeshTexture> textures) {
	Mesh mesh = {};
	mesh.vertices = std::move(vertices);
	mesh.indices  = std::move(indices);
	mesh.textures = std::move(textures);
	mesh.is_setup = false;
	return mesh;
}

void setup_mesh(Mesh* mesh) {
	// create buffers/arrays
	glGenVertexArrays(1, &mesh->VAO);
	glGenBuffers(1, &mesh->VBO);
	glGenBuffers(1, &mesh->EBO);

	glBindVertexArray(mesh->VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);

	// 
	// Set the vertex attribute pointers...
	// 
	
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));

	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bi_tangent));

	// ids
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, bone_ids));

	// weights
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));

	glBindVertexArray(0);
	mesh->is_setup = true;
}

void destroy_mesh(Mesh* mesh) {
	if (mesh->is_setup) {
		glDeleteVertexArrays(1, &mesh->VAO);
		glDeleteBuffers(1, &mesh->VBO);
		glDeleteBuffers(1, &mesh->EBO);
		mesh->is_setup = false;
	}
}

// Rendering
void draw_mesh(const Mesh *mesh, const Shader *shader) {
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < mesh->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = mesh->textures[i].type;
		if (name == "material.diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "material.specular")
			number = std::to_string(specularNr++); // transfer unsigned int to string
		else if (name == "material.normal")
			number = std::to_string(normalNr++); // transfer unsigned int to string
		else if (name == "material.height")
			number = std::to_string(heightNr++); // transfer unsigned int to string

		// now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader->id, (name + number).c_str()), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
	}

	// draw mesh
	glBindVertexArray(mesh->VAO);
	glDrawElements(GL_TRIANGLES, static_cast<u32>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}
