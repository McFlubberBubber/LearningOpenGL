#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

// Just raw, floating text that eventually fades away.
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

// Text boxes that can be drawn, but also fade away.
struct TextBox {
	FontTag tag;
	std::string text;
	float x, y;
	float scale;

	glm::vec3 text_color;
	glm::vec3 bg_color;
	float alpha;

	// @TODO: We could make it where these are irrelevant if we want to draw a
	// text box that's 'consistent' and doesn't fade away?
	float lifetime = 1.0f;
	float fade_duration = 1.0f;
	float time_elapsed = 0.0f;

	TextAlign align = TextAlign::CENTER;
	float padding;

};


// Implementing a message queue system for message logging and stuff within the application.
struct Message {
	std::string text;
	glm::vec3 text_color;
	glm::vec3 bg_color;

	float scale;
	float padding;

	float lifetime;
	float fade_duration;
	float time_elapsed;
	float alpha;
	
	float y_position;
};

struct MessageQueue {
	std::vector<Message> messages;

	// @TODO: Should these base coordinates be floats? These get positioned base on the pixel
	// so it might be good to keep these as unsigned integers.
	float base_x;
	float base_y;

	float message_spacing;
	int max_messages;
};

struct Font {
	FT_Library freetype;
	FT_Face face;
	u32 VAO, VBO; // @TODO: Should this get moved into the BufferData struct?
	
	std::map<char, Character> characters;
	std::vector<FadingText> fading_texts;
	std::vector<TextBox> text_boxes;
	
	bool is_valid;
};

// Font management
Font create_font();
bool load_font(Font* font, const char* path, u32 pixel_height);
void destroy_font(Font* font);


// Text rendering utils
void draw_text(const Font* font, const RenderingContext* context, const std::string& text, float x, float y, float scale, const glm::vec3& color, float alpha, TextAlign align, bool drop_shadow);

// Drawing fading texts
float get_string_width_in_pixels(const Font* font, const std::string& text, float scale);
void trigger_fading_text(Font* font, const FontTag tag, const std::string& text, float x, float y, float scale,	const glm::vec3& color, float lifetime, float fade_duration, TextAlign align); 
void update_and_draw_fading_texts(Font* font, const RenderingContext* context, float dt);

// Drawing text box stuff
void trigger_text_box(Font* font, const FontTag tag, const std::string& text, float x, float y, float scale, const glm::vec3& text_color, const glm::vec3& bg_color, float lifetime, float fade_duration, TextAlign align, float padding = 8.0f);

void update_and_draw_text_boxes(Font* font, const RenderingContext* ctx, float dt);

void draw_text_with_background(const Font* font, const RenderingContext* ctx, const std::string& text, float x, float y, float scale, const glm::vec3 text_color, const glm::vec3& bg_color, float alpha, TextAlign align, float padding);

// Message queue stuff
bool init_message_queue(RenderingContext* ctx);
void update_message_queue(RenderingContext* ctx);
void push_message(MessageQueue* queue, const std::string& text, float lifetime = 1.5f, float fade_duration = 0.75f);
void update_and_draw_message_queue(Font* font, RenderingContext* ctx, float dt);

 
// Internal helpers
/*
void load_char_glyphs(Font* font);
void init_font_buffers(Font* font);
void cleanup_font_resources(Font* font);
*/
