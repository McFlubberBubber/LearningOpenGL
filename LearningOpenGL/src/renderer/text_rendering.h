#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "render_context.h"
#include "core/types.h"

// Forward declarations
struct RenderingContext;

struct Character {
	u32 texture_id;				// ID handle of glyph 
	glm::ivec2 size;			// Size of glyph
	glm::ivec2 bearing;			// Offset from baseline to left/top
	u32 advance;				// Offset to advance to next glyph
};

enum class TextAlign {
	LEFT,
	CENTER,
	RIGHT,
	SCREEN_CENTER,		// Relative to the screen
};

enum class FontTag {
	TAG_DEFAULT,

	TAG_CAMERA,
	TAG_RENDER,
	TAG_ZOOM,
	TAG_DEBUG,
};

struct FadingText {
	FontTag tag;
	std::string text;
	float x, y;
	float scale;
	glm::vec3 color;
	float alpha = 1.0f;

	float lifetime 		= 1.0f;
	float fade_duration = 1.0f;
	float time_elapsed	= 0.0f;

	TextAlign align = TextAlign::LEFT;		// Default alignment
};


struct Font {
	FT_Library freetype;
	FT_Face face;
	u32 VAO, VBO;
	
	std::map<char, Character> characters;
	std::vector<FadingText> fading_texts;
	
	bool is_valid;
};

// Font management
Font create_font();
bool load_font(Font* font, const char* path, u32 pixel_height);
void destroy_font(Font* font);


// Text rendering utils
void draw_text(const Font* font, const RenderingContext* context, const std::string& text, float x, float y, float scale, const glm::vec3& color, float alpha, TextAlign align, bool drop_shadow);

float get_string_width_in_pixels(const Font* font, const std::string& text, float scale);

void trigger_fading_text(Font* font, const FontTag tag, const std::string& text, float x, float y, float scale, const glm::vec3& color, float lifetime, float fade_duration, TextAlign align); 

void update_and_draw_fading_texts(Font* font, const RenderingContext* context, float dt);


// Internal helpers
void load_char_glyphs(Font* font);
void init_font_buffers(Font* font);
void cleanup_font_resources(Font* font);
