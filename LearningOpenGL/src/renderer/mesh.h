#pragma once

// #define DO_OG_MESH

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

#include "core/types.h"

struct Shader; // Forward declaration.

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 position;
    // normal
    glm::vec3 normal;
    // texCoords
    glm::vec2 texture;
    // tangent
    glm::vec3 tangent;
    // bitangent
    glm::vec3 bi_tangent;
    //bone indexes which will influence this vertex
    int bone_ids[MAX_BONE_INFLUENCE];
    //weights from each bone
    float weights[MAX_BONE_INFLUENCE];
};

struct MeshTexture {
    unsigned int id;
    std::string type;
    std::string path;
};

struct Mesh {
    // public:
    std::vector<Vertex>      vertices;
    std::vector<u32>         indices;
    std::vector<MeshTexture> textures;

    u32 VAO;

    // private:
    u32 VBO, EBO;
    bool is_setup;
};

// Mesh management
Mesh create_mesh(std::vector<Vertex> vertices, std::vector<u32> indices, std::vector<MeshTexture> textures);
void setup_mesh(Mesh *mesh);
void destroy_mesh(Mesh *mesh);

// Rendering
void draw_mesh(const Mesh *mesh, const Shader *shader);
