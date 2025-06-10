#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"
#include "stb_image.h"


class Model {
public:
	//constructor
	Model(const char* path, bool flipUvs)
	{
		m_LoadModel(path, flipUvs);			//immediately loads the model based on path
	}

	//drawing the full model based on the amount of meshes found
	void Draw(Shader& shader) {
		for (unsigned int i = 0; i < m_meshes.size(); i++) {
			m_meshes[i].Draw(shader);
		}
	}

private:
	//model data
	std::vector<Texture> m_texturesLoaded;
	std::vector<Mesh> m_meshes;
	std::string m_directory;

	void m_LoadModel(std::string path, bool flipUvs) {
		//creating importer object to read file path and execute post processing options of ASSIMP
		Assimp::Importer importer;
		const aiScene* scene;
		if (flipUvs) {
			scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		}
		else {
			scene = importer.ReadFile(path, aiProcess_Triangulate);
		}


		//error logging if no scene exists / flags are incomplete
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}

		m_directory = path.substr(0, path.find_last_of('/'));
		m_ProcessNode(scene->mRootNode, scene);
	}

	//recursively processing each node
	void m_ProcessNode(aiNode* node, const aiScene* scene) {
		//processing all the node's meshes if there are any
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_meshes.push_back(m_ProcessMesh(mesh, scene));
		}

		//then do the same for each node's children
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			m_ProcessNode(node->mChildren[i], scene);
		}
	}


	Mesh m_ProcessMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		//processing vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			glm::vec3 vector;

			//positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;

			//normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;

			//textures
			if (mesh->mTextureCoords[0]) {				//if there are any textures in the first place
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.texture = vec;
			}
			else {										//else set default texture values to 0f
				vertex.texture = glm::vec2(0.0f);
			}
			vertices.push_back(vertex);
		}

		//processing indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		//processing materials
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			//loading diffuse maps
			std::vector<Texture> diffuseMaps = m_LoadMaterialTexture(material, aiTextureType_DIFFUSE, "textureDiffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			//loading specular maps
			std::vector<Texture> specularMaps = m_LoadMaterialTexture(material, aiTextureType_SPECULAR, "textureSpecular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	//loading the material textures based on the type that was specified
	std::vector<Texture> m_LoadMaterialTexture(aiMaterial* material, aiTextureType type, std::string typeName) {
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
			aiString str;
			material->GetTexture(type, i, &str);
			bool skip = false;
			std::string fullPath = m_directory + '/' + str.C_Str();
			
			//if the texture has been loaded previously, break the loop
			for (unsigned int j = 0; j < m_texturesLoaded.size(); j++) {
				
				//CURRENTLY: testing changes to ensure texture file names that match don't get reused in other models
				/*
				if (m_texturesLoaded[j].path == fullPath) {
					textures.push_back(m_texturesLoaded[j]);
					skip = true;
					break;
				}
				*/

				if (std::strcmp(m_texturesLoaded[j].path.data(), str.C_Str()) == 0) {
					textures.push_back(m_texturesLoaded[j]);
					skip = true;
					break;
				}
				
			}

			//if the texture has not been loaded already, then load it
			if (!skip) {
				Texture texture;
				texture.id = m_TextureFromFile(str.C_Str(), m_directory);		//old
				//texture.id = m_TextureFromFile(fullPath.c_str(), "");
				texture.type = typeName;
				texture.path = str.C_Str();
				//texture.path = fullPath;
				textures.push_back(texture);
				m_texturesLoaded.push_back(texture);		//adding to loaded textures vector
			}
		}
		return textures;
	}

	//loading textures from a file
	unsigned int m_TextureFromFile(const char* path, const std::string &directory) {
		std::string fileName = std::string(path);
		fileName = directory + '/' + fileName;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);

		if (data)
		{
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture at path: " << path << std::endl;
			stbi_image_free(data);
		}
		return textureID;
	}
};