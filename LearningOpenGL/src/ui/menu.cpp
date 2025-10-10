#include "menu.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer/render_context.h"
#include "core/time.h"

#include "debug_overlay.h"


void init_menu(Menu* menu) {
	menu->items = {
		MenuItem::RESUME,
		MenuItem::MUSIC,
		MenuItem::SCENE_SWITCH,
		MenuItem::QUIT
	};

	menu->current_item = MenuItem::RESUME;
	menu->do_music	   = true;
}


static void draw_menu_title(const RenderingContext* context) {
	const ViewportState* viewport = &context->viewport;
	const Font* font = context->assets.fonts;

	const std::string text = "LearningOpenGL";
	const float x = viewport->width  / 2.0f;
	const float y = viewport->height / 1.2f;
	const float scale = 1.0f;
	
	const glm::vec3 color = glm::vec3(0.9f, 0.9, 0.5f);
	const float alpha = 1.0f;
	const TextAlign align = TextAlign::CENTER;
	const bool do_drop_shadow = true;

	draw_text(&font[FONT_TITLE], context, text, x, y, scale, color, alpha, align, do_drop_shadow);
}


static void draw_small_text(const RenderingContext* context) {
	const ViewportState* viewport = &context->viewport;
	const Font* font = context->assets.fonts;

	// @HARDCODE: We are always using OpenGL 4.6.0, so it would constantly be the
	// same version, but there are integers in the main file that we can pass here
	std::string text = "OpenGL Version: 4.6.0";

	float x = viewport->width  - 140.0f;
	float y = viewport->height - 25.0f;

	const float scale = 1.0f;
	
	glm::vec3 color = glm::vec3(0.1f, 0.1f, 0.1f);

	const float alpha = 0.75f;
	const TextAlign align = TextAlign::CENTER;
	const bool do_drop_shadow = false;

	draw_text(&font[FONT_SMALL], context, text, x, y, scale, color, alpha, align, do_drop_shadow);
	

	// Drawing credits at the bottom of the menu
	text = "Created by McFlubberBubber - 2025";
	x = viewport->width / 2.0f;
	y = 10.0f;
	
	draw_text(&font[FONT_SMALL], context, text, x, y, scale, color, alpha, align, do_drop_shadow);

	// std::cout << "Drawing credits text at: " << x << "x" << y << std::endl;
}


std::string menu_item_to_string(const Menu* menu, MenuItem item) {
	switch (item) {
		case MenuItem::RESUME:
			return "Resume";
			
		case MenuItem::MUSIC:
			return std::string("Music: " ) + (menu->do_music ? "ON" : "OFF");

		case MenuItem::SCENE_SWITCH:
			return std::string("Current Scene: ") + (menu->render_normal_scene ? "Normal" : "Space");
			
		case MenuItem::QUIT:
			return "Quit";
			
		default:
			return "Unknown";
	}
}


static void draw_menu_choices(const RenderingContext* context, const Menu* menu) {
	const ViewportState* viewport = &context->viewport;
	const Font* font = context->assets.fonts;

	const float x = viewport->width / 2.0f;
	float y = viewport->height / 1.6f;

	const float spacing = 125.0f;
	const float scale 	= 1.0f;
	const glm::vec3 unselected_color = glm::vec3(0.5f);
	
	float t = Time::get_time();
	const float interp_speed = 5.0f;
	float blue_value = (1.0f + sin(t * interp_speed)) * 0.5f;
	glm::vec3 selected_color = glm::vec3(1.0f, 1.0f, blue_value);


	for (s32 i = 0; i < menu->items.size(); i++) {
		const bool selected = (menu->current_item == menu->items[i]);
		const glm::vec3 color = selected ? selected_color : unselected_color;

		const float alpha = 1.0f;
		const TextAlign align = TextAlign::CENTER;
		const bool do_drop_shadow = true;

		std::string text = menu_item_to_string(menu, menu->items[i]);

		draw_text(&font[FONT_BODY], context, text, x, y - (i * spacing), scale, color, alpha, align, do_drop_shadow);
	}
}


void draw_menu(RenderingContext* ctx, const Menu* menu) {
	glClearColor(0.05f, 0.2f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_menu_title(ctx);
	draw_small_text(ctx);
	draw_menu_choices(ctx, menu);

	render_debug_overlay(ctx, Time::get_delta_time());
}


void increment_menu_item(Menu* menu) {
	int next_item = (static_cast<s32> (menu->current_item) + 1) % static_cast<s32> (MenuItem::COUNT);

	menu->current_item = static_cast<MenuItem> (next_item);
}


void decrement_menu_item(Menu* menu) {
	s32 current = static_cast<s32> (menu->current_item);
	s32 total   = static_cast<s32> (MenuItem::COUNT);
	s32 prev	= (current - 1 + total) % total;

	menu->current_item = static_cast<MenuItem> (prev);
}
