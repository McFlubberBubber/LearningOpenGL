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

extern Shader font_shader;

struct Character {
	unsigned int texture_id;	// ID handle of glyph 
	glm::ivec2   size;			// Size of glyph
	glm::ivec2   bearing;		// Offset from baseline to left/top
	unsigned int advance;		// Offset to advance to next glyph
};


void init_font_buffers();
void load_font();
void cleanup_freetype();

// Utilities
void draw_text(const glm::mat4 &ortho_projection, Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);

#endif
