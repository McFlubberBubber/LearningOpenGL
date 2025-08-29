#pragma once

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>

#include "core/types.h"
#include "renderer/mesh.h"

//#include "shader.h"
//#include "render_data.h"
//#include "stb_image.h"

// Forward declaration
struct Assets;
struct Shader;

struct Model {
	std::vector<Mesh> meshes;
	std::vector<u32> texture_ids;
	std::vector<std::string> texture_paths;
	std::string directory;

	bool is_loaded;
};


// Model management
Model create_model();
bool load_model(Model* model, const char* path, bool flip_UVs, Assets* assets);
void destroy_model(Model* model);

void draw_model(const Model* model, const Shader* shader, const Assets* assets);


// Internal helper functions
bool process_node(Model* model, aiNode* node, const aiScene* scene, Assets* assets);
Mesh process_mesh(Model* model, aiMesh* mesh, const aiScene* scene, Assets* assets);
std::vector<MeshTexture> load_material_textures(Model* model, aiMaterial* material, aiTextureType type, const std::string &type_name);
u32 texture_from_file(const char* path, const std::string &directory);
