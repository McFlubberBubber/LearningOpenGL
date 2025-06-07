#pragma once

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Shader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture;
};

struct Texture {
	unsigned int id;
	std::string type;			//eg: diffuse, specular, emission
	std::string path;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int VAO;

	//constructor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		m_SetupMesh();
	}

	//assuming the uniform naming convention of textures will always be texture<type>N, where N is the number of the texture
	void Draw(Shader& shader) {
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int emissionNr = 1;

		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);			//incrementing nr of textures
			std::string number;
			std::string name = textures[i].type;
			
			//conditions for checking uniform naming
			if (name == "textureDiffuse")
				number = std::to_string(diffuseNr++);		//adding the texture number to the end of the uniform
			
			else if (name == "textureSpecular")
				number = std::to_string(specularNr++);

			else if (name == "textureEmission")
				number = std::to_string(emissionNr++);

			//setting the uniform for the mesh's material uniform with appropriate name and numbering
			shader.setInt(("u_material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		//actually drawing the mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	unsigned int m_VBO, m_EBO;
	
	void m_SetupMesh() {
		//generating arrays and buffers
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		//binding arrays
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		//vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));			//offsetof(s, m) where s = struct, m = variable member
		//vertex textures
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));

		glBindVertexArray(0);
	}
};

#endif