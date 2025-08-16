#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "menu.h"
#include "program_state.h"
#include "UserInput.h"
#include "text_rendering.h"
#include "Shader.h"
#include "Rendering.h"

// @TODO: Using global variables here from rendering files
extern Font bold_text;
extern Font italic_text;
extern Shader font_shader;

Font title_text;
Font body_text;
Font small_text;

// Using the menu that is declared in main.cpp
extern Menu menu;

void init_menu() {
	// Loading necessary fonts
	title_text		 = Font("res/fonts/Merriweather_24pt-Italic.ttf", 96);
	body_text		 = Font("res/fonts/Merriweather_24pt-Bold.ttf", 60);
	small_text		 = Font("res/fonts/Merriweather_24pt-Bold.ttf", 24);

	menu.items = {
		MenuItem::RESUME,
		MenuItem::MUSIC,
		MenuItem::QUIT
	};
}


void draw_menu_title() {
	const std::string text = "LearningOpenGL";
	const float x = RenderState::SCREEN_WIDTH / 2.0f;
	const float y = RenderState::SCREEN_HEIGHT / 1.2f;
	const float scale = 1.0f;
	
	const glm::vec3 color = glm::vec3(0.9f, 0.9, 0.5f);
	const float alpha = 1.0f;
	const TextAlign align = TextAlign::CENTER;
	const bool do_drop_shadow = true;

	title_text.draw_text(font_shader, text, x, y, scale, color, alpha, align, do_drop_shadow); 	
}


void draw_small_text() {

	// @HARDCODE: We are always using OpenGL 4.6.0, so it would constantly be the
	// same version, but there are integers in the main file that we can pass here
	std::string text = "OpenGL Version: 4.6.0";
	float x = RenderState::SCREEN_WIDTH - 140.0f;
	float y = RenderState::SCREEN_HEIGHT - 25.0f;
	const float scale = 1.0f;
	
	const glm::vec3 color = glm::vec3(0.1f, 0.1f, 0.1f);
	const float alpha = 0.75f;
	const TextAlign align = TextAlign::CENTER;
	const bool do_drop_shadow = false;
	

	small_text.draw_text(font_shader, text, x, y, scale, color, alpha, align, do_drop_shadow);


	// Drawing credits at the bottom of the menu
	text = "Created by McFlubberBubber - 2025";
	x = RenderState::SCREEN_WIDTH / 2.0f;
	y = 10.0f;
	
	small_text.draw_text(font_shader, text, x, y, scale, color, alpha, align, do_drop_shadow);
}


std::string menu_item_to_string(const Menu& menu, MenuItem item) {
	switch (item) {
		case MenuItem::RESUME:
			return "Resume";
			
		case MenuItem::MUSIC:
			return std::string("Music: " ) + (menu.do_music ? "ON" : "OFF");
			
		case MenuItem::QUIT:
			return "Quit";
			
		default:
			return "Unknown";
	}
}


void draw_menu_choices() {
	const float x = RenderState::SCREEN_WIDTH / 2.0f;
	float y = RenderState::SCREEN_HEIGHT / 1.6f;
	
	const float spacing = 125.0f;
	const float scale 	= 1.0f;

	for (int32_t i = 0; i < menu.items.size(); i++) {
		const bool selected = (menu.current_item == menu.items[i]);
		const glm::vec3 color = selected ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);

		const float alpha = 1.0f;
		const TextAlign align = TextAlign::CENTER;
		const bool do_drop_shadow = true;

		std::string text = menu_item_to_string(menu, menu.items[i]);

		body_text.draw_text(font_shader, text, x, y - (i * spacing), scale, color, alpha, align, do_drop_shadow);

	}
}


void draw_menu() {
	glClearColor(0.05f, 0.2f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_menu_title();
	draw_small_text();
	draw_menu_choices();
}


void increment_menu_item(Menu& menu) {
	int next_item = (static_cast<int32_t> (menu.current_item) + 1) % static_cast<int32_t> (MenuItem::COUNT);

	menu.current_item = static_cast<MenuItem> (next_item);
}


void decrement_menu_item(Menu& menu) {
	int32_t current = static_cast<int32_t> (menu.current_item);
	int32_t total   = static_cast<int32_t> (MenuItem::COUNT);
	int32_t prev	= (current - 1 + total) % total;

	menu.current_item = static_cast<MenuItem> (prev);
}
