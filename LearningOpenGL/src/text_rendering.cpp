#include "text_rendering.h"
#include <iostream>

unsigned int font_VAO, font_VBO;

FT_Library freetype;
FT_Face font_face;

Shader font_shader;

std::map<char, Character> characters;


void init_font_buffers () {
	glGenVertexArrays(1, &font_VAO);
	glGenBuffers(1,		 &font_VBO);

	glBindVertexArray(font_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, font_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void load_font() {	
	font_shader = Shader("res/shaders/font.vert", "res/shaders/font.frag");
	font_shader.useProgram();

	
	if (FT_Init_FreeType(&freetype)) {
		std::cout << "ERROR::FREETYPE: Could not initialize FreeType Library!\n";
		return;
	}

	if (FT_New_Face(freetype, "res/fonts/Merriweather_24pt-Regular.ttf", 0 , &font_face)) {
		std::cout << "ERORR::FREETYPE: Could not load font!\n";
		return;
	}

	// Numbers represent width and height, width is kept at 0 so it can
	// scale dynamically with the height we give
	FT_Set_Pixel_Sizes(font_face, 0, 48);

	// Disabling byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		

	// Loading character glyphs from 0-128 ASCII 
	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(font_face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYPE: Failed to load glyph!\n";
			continue;
		}

		// Generating the texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font_face->glyph->bitmap.width, font_face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, font_face->glyph->bitmap.buffer);

		// Setting the texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Storing each character for later use
		Character character = {
			texture,
			glm::ivec2(font_face->glyph->bitmap.width, font_face->glyph->bitmap.rows),
			glm::ivec2(font_face->glyph->bitmap_left, font_face->glyph->bitmap_top),
			font_face->glyph->advance.x
		};
		characters.insert(std::pair<char, Character>(c, character));
	}
}

void cleanup_freetype() {
	FT_Done_Face(font_face);
	FT_Done_FreeType(freetype);
}

void draw_text(const glm::mat4 &ortho_projection, Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color) {
	shader.useProgram();
	shader.setMat4("u_projection", ortho_projection);
	shader.setVec3("text_color", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(font_VAO);

	// Iterating through all the characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character character = characters[*c];

		float x_pos = x + character.bearing.x * scale;
		float y_pos = y - (character.size.y - character.bearing.y) * scale;

		float w = character.size.x * scale;
		float h = character.size.y * scale;

		// Updating the VBO for each character
		float vertices[6][4] = {
			{ x_pos    , y_pos + h, 0.0f, 0.0f },
			{ x_pos    , y_pos	  , 0.0f, 1.0f },
			{ x_pos + w, y_pos    , 1.0f, 1.0f },

			{ x_pos,     y_pos + h, 0.0f, 0.0f },
			{ x_pos + w, y_pos    , 1.0f, 1.0f },
			{ x_pos + w, y_pos + h, 1.0f, 0.0f },
		};

		// Rendering the glyph over each quad
		glBindTexture(GL_TEXTURE_2D, character.texture_id);
		glBindBuffer(GL_ARRAY_BUFFER, font_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Advancing cursors for the next glyph
		x += (character.advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
