#pragma once
#ifndef TEXT_RENDERING_H
#define TEXT_RENDERING_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

struct Character {
	unsigned int texture_id;	// ID handle of glyph 
	glm::ivec2   size;			// Size of glyph
	glm::ivec2   bearing;		// Offset from baseline to left/top
	unsigned int advance;		// Offset to advance to next glyph
};

class Font {
public:
	uint32_t font_VAO = 0;
	uint32_t font_VBO = 0;
	bool is_valid = false;
	
	FT_Library freetype = nullptr;
	FT_Face font_face	= nullptr;

	std::map<char, Character> characters;

	
	// Default constructor + destructor
	Font() = default;
	~Font();
	

	// Constructor to font paths, and set pixel_height
	Font(const char* path, uint32_t pixel_height);


	// @TODO: I gotta look into more about these other constructors
	// because I have no idea what these delete / copy / move
	// constructors are doing

	// Delete copy
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	// Adding move constructors
	Font(Font&& other) noexcept;
	Font& operator=(Font&& other) noexcept;


	// Utility Functions
	void init_font_buffers();
	void load_character_glyphs();
	
	void draw_text(const glm::mat4 &ortho_projection, Shader &shader, const std::string &text, float x, float y, float scale, const glm::vec3 &color);
	
	void cleanup_freetype();
};


#endif
