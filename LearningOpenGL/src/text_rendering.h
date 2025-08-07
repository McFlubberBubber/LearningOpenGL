#pragma once
#ifndef TEXT_RENDERING_H
#define TEXT_RENDERING_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
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


struct FadingText {
	std::string tag;
	std::string text;
	float x, y;
	float scale;
	glm::vec3 color;
	float alpha = 1.0f;

	float lifetime 		= 1.0f;
	float fade_duration = 1.0f;
	float time_elapsed	= 0.0f;

	// Constructor for FadingText
	FadingText(const std::string& tag_name, const std::string& txt, float x_pos, float y_pos, float scl, const glm::vec3& col, float life, float fade)
		: tag(tag_name), text(txt), x(x_pos), y(y_pos), scale(scl), color(col), alpha(1.0f), lifetime(life), fade_duration(fade), time_elapsed(0.0f) {} 
};



class Font {
public:
	uint32_t font_VAO = 0;
	uint32_t font_VBO = 0;
	bool is_valid = false;
	
	FT_Library freetype = nullptr;
	FT_Face font_face	= nullptr;

	std::map<char, Character> characters;
	std::vector<FadingText> fading_texts;
	
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
	

	void trigger_fading_text(const std::string& tag, const std::string& text, float x, float y, float scale, const glm::vec3& color, float lifetime, float fade_duration);

	void update_and_draw_fading_texts(const glm::mat4& ortho_projection, Shader& shader, float delta_time);
	
	void draw_text(const glm::mat4 &ortho_projection, Shader &shader, const std::string &text, float x, float y, float scale, const glm::vec3 &color, float alpha);


	void cleanup_freetype();
};


#endif
