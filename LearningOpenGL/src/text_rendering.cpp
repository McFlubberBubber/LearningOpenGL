#include "text_rendering.h"
#include <iostream>


// Constructor that sets a font to a font object instance
Font::Font(const char* font_path, uint32_t pixel_height){
	// Error logging for free type 	
	if (FT_Init_FreeType(&freetype)) {
		std::cout << "ERROR::FREETYPE: Could not initialize FreeType Library!\n";
		font_face = nullptr;
		return;
	}

	if (FT_New_Face(freetype, font_path, 0 , &font_face)) {
		std::cout << "ERORR::FREETYPE: Could not load font: " << font_path << "!\n";
		return;
	} else {
		std::cout << "Font loaded at path: " << font_path << "\n";
	}

	// Numbers represent width and height, width is kept at 0 so it can
	// scale dynamically with the height we give
	FT_Set_Pixel_Sizes(font_face, 0, pixel_height);

	// Disabling byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		

	load_character_glyphs();
	init_font_buffers();
	
	is_valid = true;
}


// Move constructor
Font::Font(Font&& other) noexcept {
	font_VAO = other.font_VAO;
	font_VBO = other.font_VBO;
	is_valid = other.is_valid;

	freetype  = other.freetype;
	font_face = other.font_face;

	characters = std::move(other.characters);

	// Invalidate the moved-from object
	other.font_VAO = 0;
	other.font_VBO = 0;
	other.is_valid = false;

	other.freetype  = nullptr;
	other.font_face = nullptr;
}

Font& Font::operator=(Font&& other) noexcept {
	if (this != &other) {
		cleanup_freetype();

		font_VAO = other.font_VAO;
		font_VBO = other.font_VBO;
		is_valid = other.is_valid;

		freetype  = other.freetype;
		font_face = other.font_face;

		characters = std::move(other.characters);

		// Invalidate the moved-from object
		other.font_VAO = 0;
		other.font_VBO = 0;
		other.is_valid = false;

		other.freetype  = nullptr;
		other.font_face = nullptr;
	}
	return *this;
}


void Font::init_font_buffers () {
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


// Functions to draw the fading texts
void Font::trigger_fading_text(const std::string& tag, const std::string& text, float x, float y, float scale, const glm::vec3& color, float lifetime, float fade_duration) {

	// @Speed
	// Check for matching tags to reuse fading texts
	for (auto& ft : fading_texts) {
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
		}
	}

	// Otherwise, add a new fading text
	fading_texts.emplace_back(tag, text, x, y, scale, color, lifetime, fade_duration);
}

void Font::update_and_draw_fading_texts(const glm::mat4& ortho_projection, Shader& shader, float delta_time) {
	for ( auto it = fading_texts.begin(); it != fading_texts.end(); ) {
		it->time_elapsed += delta_time;

		if (it->time_elapsed >= it->lifetime + it->fade_duration) {
			it = fading_texts.erase(it);	// Once the text is fully faded
			continue;
		}


		// Updating the alpha
		float current_alpha = 1.0f;
		if (it->time_elapsed > it->lifetime) {
			float fade_progress = (it->time_elapsed - it->lifetime) / it->fade_duration;
			current_alpha = glm::clamp(1.0f - fade_progress, 0.0f, 1.0f);
		}
	

	   	// Drawing the text to the screen
		draw_text(ortho_projection, shader, it->text, it->x, it->y, it->scale, it->color, current_alpha);
		++it;
	}
}


// Function to draw text normally within render loop
void Font::draw_text(const glm::mat4 &ortho_projection, Shader &shader, const std::string &text, float x, float y, float scale, const glm::vec3 &color, float alpha) {
	shader.useProgram();
	shader.setMat4("u_projection", ortho_projection);
	shader.setVec3("text_color", color);
	shader.setFloat("text_alpha", alpha);
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



// Destructor that will clean up FreeType resources
Font::~Font(){
	cleanup_freetype();
}



void Font::load_character_glyphs(){
	// Loading character glyphs from 0-128 ASCII 
	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(font_face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYPE: Failed to load glyph!\n";
			continue;
		}

		// Generating the texture
		uint32_t texture;
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



void Font::cleanup_freetype() {
	if (is_valid && font_face) {
		FT_Done_Face(font_face);
		font_face = nullptr;
	}

	if (is_valid && freetype) {
		FT_Done_FreeType(freetype);
		freetype = nullptr;
	}
	
	glDeleteVertexArrays(1, &font_VAO);
	glDeleteBuffers(1, &font_VBO);
}
