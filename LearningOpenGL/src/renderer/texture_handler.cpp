#include "texture_handler.h"

#include "renderer/render_data.h"
#include "stb_image.h"

bool add_2D_texture(Assets* assets, TextureType texture, const char* path) {
	u32 texture_id = 0;
	texture_id = load_2D_texture(path);

	if (texture_id != 0) {
		assets->textures[texture] = texture_id;
		return true;
	}

	return false;
}


bool add_cubemap_texture(Assets* assets, TextureType texture, const std::vector<std::string> &faces) {
	u32 texture_id = 0;
	if (faces.size() != 6) {
		std::cout << "ERROR: Loading cubemaps requires exactly 6 texture faces." << std::endl;
		return false;
	}

	texture_id = load_cubemap_texture(faces);
	if (texture_id != 0) {
		assets->textures[texture] = texture_id;
		return true;
	}

	return false;
}


// Internal helper functions - private?
u32 load_2D_texture(const char* path) {
	u32 textureID{};
	s32 width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	glGenTextures(1, &textureID);

	if (data) {
		GLenum textureFormat{};
		if (nrComponents == 1)
			textureFormat = GL_RED;
		else if (nrComponents == 3)
			textureFormat = GL_RGB;
		else if (nrComponents == 4)
			textureFormat = GL_RGBA;

		//binding the texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, width, height, 0, textureFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);


		if (textureFormat != GL_RGBA) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Logging success / errors loading textures
		std::cout << "Texture loaded at path: " << path << "\n";
		stbi_image_free(data);
	}
	else {
		std::cout << "ERROR: Failed to load texture at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


u32 load_cubemap_texture(const std::vector<std::string>& faces) {
	stbi_set_flip_vertically_on_load(false);

	u32 texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	s32 width, height, nr_channels;
	for (u32 i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nr_channels, 0);

		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
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
