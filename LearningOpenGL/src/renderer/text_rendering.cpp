#include "text_rendering.h"

#include <iostream>

#include "renderer/render_context.h"
#include "renderer/shader.h"
//#include "core/program_state.h"


// Font management functions
Font create_font() {
	Font font = {};
	
	font.freetype = nullptr;
	font.face	  = nullptr;

	font.VAO = 0;
	font.VBO = 0;
	font.is_valid = false;

	return font;
}


bool load_font(Font* font, const char* path, u32 pixel_height) {
	// Initializing freetype
	if (FT_Init_FreeType(&font->freetype)) {
		std::cout << "ERROR::FREETYPE: Could not initialize freetype library!" << std::endl;
		return false;
	}

	// Loading font face
	if (FT_New_Face(font->freetype, path, 0, &font->face)) {
		std::cout << "ERROR::FREETYPE: Could not load font at path: " << path << std::endl;
		FT_Done_FreeType(font->freetype);
		font->freetype = nullptr;
		return false;
	}

	std::cout << "Font loaded at path: " << path << std::endl;


	// Setting freetype configs
	FT_Set_Pixel_Sizes(font->face, 0, pixel_height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	load_char_glyphs(font);
	init_font_buffers(font);

	font->is_valid = true;
	return true;
}


void destroy_font(Font* font) {
	cleanup_font_resources(font);
	*font = create_font();		// Resetting to clean state
}


// Functions to draw text on the screen
void draw_text(const Font* font, const RenderingContext* context, const std::string& text, float x, float y, float scale, const glm::vec3& color, float alpha, TextAlign align, bool drop_shadow) {
	if (!font->is_valid) return;
	// We constantly use the one SHADER_FONT so we just keep using it.
	// const auto shader	 = &context->assets.shaders[SHADER_FONT];
	const Shader* shader 	 = &context->assets.shaders[SHADER_FONT];

	const glm::mat4 ortho 	 = context->viewport.ortho_projection;
	const float screen_width = (float)context->viewport.width;

	use_shader(shader);
	set_mat4(shader, "projection", ortho);
	set_vec3(shader, "text_color", color);
	set_float(shader, "text_alpha", alpha);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(font->VAO);

	// Calculating alignment offset
	float total_width = get_string_width_in_pixels(font, text, scale);
	switch (align) {
	case TextAlign::CENTER:
		x -= total_width / 2.0f;
		break;
	case TextAlign::RIGHT:
		x -= total_width;
		break;
	case TextAlign::SCREEN_CENTER:
		x = (screen_width - total_width) / 2.0f;
		break;
	case TextAlign::LEFT:
		break;
	default:
		break;
	}


	// Lambda function for drawing each glyph
	auto draw_glyph = [&](float verts[6][4], const glm::vec3& color, float a) {
		set_vec3(shader, "text_color", color);
		set_float(shader, "text_alpha", alpha);

		glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 4, verts);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	};

	// Rendering each char
	for (auto c = text.begin(); c != text.end(); c++) {
		auto char_it = font->characters.find(*c);
		if (char_it == font->characters.end()) continue;
		Character character = char_it->second;

		float x_pos = x + character.bearing.x * scale;
		float y_pos = y - (character.size.y - character.bearing.y) * scale;
		float w = character.size.x * scale;
		float h = character.size.y * scale;
		
		// Vertex data for each quad
		float vertices[6][4] = {
			{ x_pos,     y_pos + h, 0.0f, 0.0f },
            { x_pos,     y_pos,     0.0f, 1.0f },
            { x_pos + w, y_pos,     1.0f, 1.0f },
            { x_pos,     y_pos + h, 0.0f, 0.0f },
            { x_pos + w, y_pos,     1.0f, 1.0f },
            { x_pos + w, y_pos + h, 1.0f, 0.0f },
		};

		glBindTexture(GL_TEXTURE_2D, character.texture_id);


		// Drawing the drop shadow (if needed)
		if (drop_shadow) {
			float shadow_offset = 2.0f;
			float shadow_verts[6][4];

			for (u32 i = 0; i < 6; i++) {
				shadow_verts[i][0] = vertices[i][0] + shadow_offset;
				shadow_verts[i][1] = vertices[i][1] - shadow_offset;
				shadow_verts[i][2] = vertices[i][2];
				shadow_verts[i][3] = vertices[i][3];
			}

			draw_glyph(shadow_verts, glm::vec3(0.0f), alpha * 0.75f);
		}

		// Drawing the main text
		draw_glyph(vertices, color, alpha);

		// Advancing the cursor to the next glyph
		x += (character.advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


float get_string_width_in_pixels(const Font* font, const std::string& text, float scale) {
	float width = 0.0f;
	if (!font->is_valid) return width;

	// Iterating through character in the text
	for (auto c = text.begin(); c != text.end(); c++) {
		if (font->characters.find(*c) != font->characters.end()) {
			Character character = font->characters.at(*c);
			width += (character.advance >> 6) * scale;
		}
	}

	return width;
}


void trigger_fading_text(Font* font, const FontTag tag, const std::string& text, float x, float y, float scale, const glm::vec3& color, float lifetime, float fade_duration, TextAlign align) {
	// Checking if there is an existing fading text with the same tag
	for (auto& ft : font->fading_texts) {
		if(ft.tag == tag) {
			// Updating ft with new data
			ft.text  = text;
			ft.x 	 = x;
			ft.y	 = y;
			ft.scale = scale;
			ft.color = color;
			ft.alpha = 1.0f;

			ft.lifetime 	 = lifetime;
			ft.fade_duration = fade_duration;
			ft.time_elapsed  = 0.0f;

			ft.align = align;
			return;
		}
	}

	// Otherwise, add a new fading text
	FadingText fading_text = {};
	fading_text.tag 		= tag;
	fading_text.text	    = text;
	fading_text.x			= x;
	fading_text.y			= y;
	fading_text.scale		= scale;
	fading_text.color		= color;
	fading_text.alpha		= 1.0f;

	fading_text.lifetime	  = lifetime;
	fading_text.fade_duration = fade_duration;
	fading_text.time_elapsed  = 0.0f;

	fading_text.align = align;
	
	font->fading_texts.push_back(fading_text);
}


void update_and_draw_fading_texts(Font* font, const RenderingContext* context, float dt) {
	// For now, all fading texts will have a drop shadow
	bool do_drop_shadow  = true;

	for (auto it = font->fading_texts.begin(); it != font->fading_texts.end();) {
		it->time_elapsed += dt;

		// Remove any fully faded texts
		if (it->time_elapsed >= it->lifetime + it->fade_duration) {
			it = font->fading_texts.erase(it);
			continue;
		}

		// Calculate the current alpha
		float current_alpha = 1.0f;
		if (it->time_elapsed > it->lifetime) {
			float fade_progress = (it->time_elapsed - it->lifetime) / it->fade_duration;
			current_alpha = glm::clamp(1.0f - fade_progress, 0.0f, 1.0f);
		}

		// Drawing the fading text
		draw_text(font, context, it->text, it->x, it->y, it->scale, it->color, current_alpha, it->align, do_drop_shadow);

		++it;
	}
}


// Internal helper functions
void load_char_glyphs(Font* font) {
	// Loading ASCII chars
	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(font->face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYPE: Failed to load glyph at character: " << (int)c << std::endl;
			continue;
		}

		// Generating the texture for each glyph
		u32 texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
					 font->face->glyph->bitmap.width,
					 font->face->glyph->bitmap.rows,
					 0, GL_RED, GL_UNSIGNED_BYTE,
					 font->face->glyph->bitmap.buffer);

		// Setting texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Storing character data
		Character character = {};
		character.texture_id = texture;
	
		character.size = glm::ivec2(font->face->glyph->bitmap.width,
									font->face->glyph->bitmap.rows);
	
		character.bearing = glm::ivec2(font->face->glyph->bitmap_left,
									   font->face->glyph->bitmap_top);

		character.advance = font->face->glyph->advance.x;

		font->characters.insert(std::pair<char, Character> (c, character));
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Initializing font buffers
// Probably does not need to be stored into the buffers of the assets structure
void init_font_buffers(Font* font) {
	glGenVertexArrays(1, &font->VAO);
	glGenBuffers(1, 	 &font->VBO);

	glBindVertexArray(font->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void cleanup_font_resources(Font* font) {
	if (!font->is_valid) return;

	// Cleanup character textures
	for (auto& pair : font->characters) {	// @TODO: Understand this.
		glDeleteTextures(1, &pair.second.texture_id);
	}
	font->characters.clear();

	// Cleanup on OpenGL resources
 	if (font->VAO != 0) {
		glDeleteVertexArrays(1, &font->VAO);
		font->VAO = 0;
	}

 	if (font->VBO != 0) {
		glDeleteVertexArrays(1, &font->VBO);
		font->VBO = 0;
	}

	// Cleanup on freetype resources
	if (font->face) {
		FT_Done_Face(font->face);
		font->face = nullptr;
	}

	if (font->freetype) {
		FT_Done_FreeType(font->freetype);
		font->freetype = nullptr;
	}

	// Cleanup on the fading texts
	font->fading_texts.clear();
	font->is_valid = false;

}
