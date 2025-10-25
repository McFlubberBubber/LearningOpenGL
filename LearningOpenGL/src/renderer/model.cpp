#include "model.h"

#include "stb_image.h"

#include "renderer/shader.h"		// For shader definition
#include "renderer/render_data.h"	// For assets definition
#include "renderer/mesh.h"			// For mesh definition

// Model management
Model create_model() {
	Model model = {};
	model.is_loaded = false;
	return model;
}

bool load_model(Model* model, const char* path, bool flip_UVs, Assets* assets) {
	Assimp::Importer importer;
	const aiScene* scene;

	// If we need to flip the UVs
	if (flip_UVs) {
		// scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	}
	else {
		// scene = importer.ReadFile(path, aiProcess_Triangulate);
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	}

	// Error handling
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return false;
	}

	model->directory = std::string(path).substr(0, std::string(path).find_last_of('/'));

	if (!process_node(model, scene->mRootNode, scene, assets))
		return false;


	// Setup all meshes after loading
	for (u32 i = 0; i < model->meshes.size(); i++) {
		setup_mesh(&model->meshes[i]);
	}

	model->is_loaded = true;
	// std::cout << "Model loaded at path: " << path << std::endl;
	return true;
}


void destroy_model(Model* model) {
	// Destroying meshes
	for (u32 i = 0; i < model->meshes.size(); ++i) {
		destroy_mesh(&model->meshes[i]);
	}

	// Cleaning up textures
	for (u32 i = 0; i < model->texture_ids.size(); ++i) {
		glDeleteTextures(1, &model->texture_ids[i]);
	}

	model->meshes.clear();
	model->texture_ids.clear();
	model->texture_paths.clear();
	model->is_loaded = false;
}


void draw_model(const Model* model, const Shader* shader) {
	if (!model->is_loaded) return;

	for (u32 i = 0; i < model->meshes.size(); i++) {
		draw_mesh(&model->meshes[i], shader);
	}
}


// Internal helper functions
bool process_node(Model* model, aiNode* node, const aiScene* scene, Assets* assets) {
	// Process all the node's meshes
	for (u32 i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model->meshes.push_back(process_mesh(model, mesh, scene, assets));
	}

	// Do the same for the node's children
	for (u32 i = 0; i < node->mNumChildren; i++) {
		if (!process_node(model, node->mChildren[i], scene, assets)) {
			return false;
		}
	}
	return true;
}


Mesh process_mesh(Model* model, aiMesh* mesh, const aiScene* scene, Assets* assets) {
	std::vector<Vertex> vertices;
	std::vector<u32> indices;
	std::vector<MeshTexture> textures;

	// Processing the vertices
	for (u32 i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;

		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		// Normals
		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}

		// Textures
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texture = vec;
		}
		else {
			vertex.texture = glm::vec2(0.0f);
		}
		vertices.push_back(vertex);
	}

	// @Speed? Processing the indices
	for (u32 i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (u32 j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<MeshTexture> diffuse_maps = load_material_textures(model, material, aiTextureType_DIFFUSE, "diffuse");
	textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

	std::vector<MeshTexture> specular_maps = load_material_textures(model, material, aiTextureType_SPECULAR, "specular");
	textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

	std::vector<MeshTexture> normalMaps = load_material_textures(model, material, aiTextureType_HEIGHT, "normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<MeshTexture> height_maps = load_material_textures(model, material, aiTextureType_AMBIENT, "height");
	textures.insert(textures.end(), height_maps.begin(), height_maps.end());

	return create_mesh(vertices, indices, textures);
}


std::vector<MeshTexture> load_material_textures(Model* model, aiMaterial* material, aiTextureType type, const std::string& type_name) {
	std::vector<MeshTexture> textures;
	u32 texture_count = material->GetTextureCount(type);
	// std::cout << "Loading " << texture_count << " " << type_name << " textures" << std::endl;

	for (u32 i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(type, i, &str);

		// Logging
		// std::cout << "  Texture path from material: " << str.C_Str() << std::endl;
		// std::cout << "  Model directory: " << model->directory << std::endl;

		bool skip = false;
		std::string full_path = model->directory + '/' + str.C_Str();
		// std::cout << "  Full path attempted: " << full_path << std::endl;


		// Checking if a texture has been loaded previously
		for (u32 j = 0; j < model->texture_paths.size(); j++) {
			if (model->texture_paths[j] == std::string(str.C_Str())) {
				MeshTexture texture;
				texture.id = model->texture_ids[j];
				texture.type = type_name;
				texture.path = str.C_Str();
				textures.push_back(texture);
				skip = true;
				break;
			}
		}

		// If the texture hasn't been loaded before, then load it
		if (!skip) {
			MeshTexture texture;
			texture.id = texture_from_file(str.C_Str(), model->directory);
			// std::cout << "  Loaded texture: " << str.C_Str() << " with ID: " << texture.id << std::endl;

			texture.type = type_name;
			texture.path = str.C_Str();
			textures.push_back(texture);

			// Adding to loaded texture cache
			model->texture_ids.push_back(texture.id);
			model->texture_paths.push_back(texture.path);
		}
	}
	return textures;
}


u32 texture_from_file(const char* path, const std::string& directory) {
	std::string file_name = std::string(path);
	file_name = directory + '/' + file_name;

	u32 texture_id;
	glGenTextures(1, &texture_id);

	s32 width, height, nr_components;
	unsigned char* data = stbi_load(file_name.c_str(), &width, &height, &nr_components, 0);

	if (data) {
		std::cout << "Successfully loaded: " << file_name << " (" << width << "x" << height << ", " << nr_components << " components)" << std::endl;

		GLenum texture_format{};
		GLenum data_format{}; // This used to be relevant for when we were loading textures in SRGB format.
		if (nr_components == 1) {
			texture_format = GL_RED;
			data_format	   = GL_RED;
		}
		
		else if (nr_components == 3) {
			texture_format = GL_RGB;
			data_format    = GL_RGB;
		}

		else if (nr_components == 4) {
			texture_format = GL_RGBA;
			data_format    = GL_RGBA;
		}
		
		// Binding texture
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, texture_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Texture wrapping + mipmapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	}
	else {
		std::cout << "ERROR: Failed to load texture at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return texture_id;
}
