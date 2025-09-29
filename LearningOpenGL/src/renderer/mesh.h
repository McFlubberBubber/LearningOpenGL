#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

#include "core/types.h"

constexpr s32 MAX_BONE_INFLUENCE = 4;

struct Shader;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture;
};

struct MeshTexture {
	u32 id;
	std::string type;	// "texture_diffuse", "texture_specular", etc.
	std::string path;	
};


struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<u32> indices;
	std::vector<MeshTexture> textures;

	u32 VAO;
	u32 VBO, EBO;

	bool is_setup;	
};

// Mesh management
Mesh create_mesh(std::vector<Vertex> vertices, std::vector<u32> indices, std::vector<MeshTexture> textures);
void setup_mesh(Mesh* mesh);
void destroy_mesh(Mesh* mesh);

// Mesh rendering
void draw_mesh(const Mesh* mesh, const Shader* shader);
