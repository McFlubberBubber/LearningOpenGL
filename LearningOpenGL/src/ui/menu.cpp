#include "menu.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer/render_context.h"
#include "core/time.h"

#include "debug_overlay.h"

// Internal function prototypes
static void draw_menu_title(const RenderingContext* context);
static void draw_small_text(const RenderingContext* context);
static void draw_menu_choices(const RenderingContext* context, const Menu* menu);
static void draw_options(const RenderingContext* ctx, const Menu* menu);

void init_menu(Menu* menu) {
	menu->main.items = {
		MenuItem::RESUME,
		MenuItem::OPTIONS,
		MenuItem::SCENE_SWITCH,
		MenuItem::QUIT
	};

	menu->options.items = {
		OptionsItem::MUSIC,
		OptionsItem::GAMMA,
		OptionsItem::MULTISAMPLING,
		OptionsItem::BACK
	};
}

void draw_menu(RenderingContext* ctx, Menu* menu) {
	glClearColor(0.05f, 0.2f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_menu_title(ctx);
	draw_small_text(ctx);
	
	if (menu->current_page == MenuPage::MAIN)
		draw_menu_choices(ctx, menu);
	else if (menu->current_page == MenuPage::OPTIONS)
		draw_options(ctx, menu);

	if (ctx->debug_mode)
		render_debug_overlay(ctx, Time::get_delta_time());
}

void increment_menu_item(Menu* menu) {
	int next_item = 0;

	// Main menu page
	if (menu->current_page == MenuPage::MAIN) {
		next_item = (static_cast<s32> (menu->main.current_item) + 1) % static_cast<s32> (MenuItem::COUNT);
		menu->main.current_item = static_cast<MenuItem>(next_item);
	}

	// Options page
	else {
		next_item = (static_cast<s32> (menu->options.current_item) + 1) % static_cast<s32> (OptionsItem::COUNT);
		menu->options.current_item = static_cast<OptionsItem>(next_item);
	}
}

void decrement_menu_item(Menu* menu) {
	// Main menu page
	if (menu->current_page == MenuPage::MAIN) {
		s32 current = static_cast<s32> (menu->main.current_item);
		s32 total   = static_cast<s32> (MenuItem::COUNT);
		s32 prev	= (current - 1 + total) % total;

		menu->main.current_item = static_cast<MenuItem>(prev);
	}
	
	// Options page
	else {
		s32 current = static_cast<s32> (menu->options.current_item);
		s32 total   = static_cast<s32> (OptionsItem::COUNT);
		s32 prev	= (current - 1 + total) % total;

		menu->options.current_item = static_cast<OptionsItem>(prev);
 	}
}


// ----- Internal Functions -----
static std::string menu_item_to_string(const Menu* menu, MenuItem item) {
	switch (item) {
	case MenuItem::RESUME:
		return "Resume";
			
	case MenuItem::SCENE_SWITCH:
		return std::string("Current Scene: ") + (menu->render_normal_scene ? "Normal" : "Space");

	case MenuItem::OPTIONS:
		return "Options";
			
	case MenuItem::QUIT:
		return "Quit";
			
	default:
		return "Unknown";
 	}
}

static std::string option_to_string(const Menu* menu, OptionsItem item) {
	std::ostringstream oss;

	switch (item) {
	case OptionsItem::MUSIC:
		return std::string("Music: ") + (menu->do_music ? "ON" : "OFF");
				
	// @TODO: These gamma values may need to get adjusted from a simple 0-100%
	case OptionsItem::GAMMA:
		oss << "Gamma: " << std::fixed << std::setw(6) << std::setprecision(2) << menu->gamma;
		return oss.str();

	case OptionsItem::MULTISAMPLING:
		return std::string("Multisampling: ") + (menu->do_multisampling ? "ON" : "OFF");
		
	case::OptionsItem::BACK:
		return "Back";

	default:
		return "Unknown";
	}
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

	std::string text = "OpenGL Version: "
		+ std::to_string(context->app.GL_MAJOR_VER) + "."
		+ std::to_string(context->app.GL_MINOR_VER) + "."
		+ std::to_string(context->app.GL_BABY_VER);

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

static void draw_menu_choices(const RenderingContext* context, const Menu* menu) {
	const ViewportState* viewport = &context->viewport;
	const Font* font = context->assets.fonts;

	const float x = viewport->width / 2.0f;
	float y = viewport->height / 1.6f;

	const float spacing = 100.0f;
	const float scale 	= 0.9f;
	const glm::vec3 unselected_color = glm::vec3(0.5f);
	
	float t = Time::get_time();
	const float interp_speed = 5.0f;
	float blue_value = (1.0f + sin(t * interp_speed)) * 0.5f;
	glm::vec3 selected_color = glm::vec3(1.0f, 1.0f, blue_value);


	for (s32 i = 0; i < menu->main.items.size(); i++) {
		const bool selected = (menu->main.current_item == menu->main.items[i]);
		const glm::vec3 color = selected ? selected_color : unselected_color;

		const float alpha = 1.0f;
		const TextAlign align = TextAlign::CENTER;
		const bool do_drop_shadow = true;

		std::string text = menu_item_to_string(menu, menu->main.items[i]);

		draw_text(&font[FONT_BODY], context, text, x, y - (i * spacing), scale, color, alpha, align, do_drop_shadow);

	}
}

static void draw_options(const RenderingContext* ctx, const Menu* menu) {
	auto vp = &ctx->viewport;
//	auto font = &ctx->assets.fonts;
	auto font = ctx->assets.fonts;

	const float x = vp->width / 2.0f;
	float y		  = vp->height / 1.6f;

	const float spacing = 100.0f;
	const float scale 	= 0.9f;
	const glm::vec3 unselected_color = glm::vec3(0.5f);
	
	float t = Time::get_time();
	const float interp_speed = 5.0f;
	float blue_value = (1.0f + sin(t * interp_speed)) * 0.5f;
	glm::vec3 selected_color = glm::vec3(1.0f, 1.0f, blue_value);

	for (s32 i = 0; i < menu->options.items.size(); i++) {
		const bool selected = (menu->options.current_item == menu->options.items[i]);
		const glm::vec3 color = selected ? selected_color : unselected_color;

		const float alpha = 1.0f;
		const TextAlign align = TextAlign::CENTER;
		const bool do_drop_shadow = true;

		std::string text = option_to_string(menu, menu->options.items[i]);

		draw_text(&font[FONT_BODY], ctx, text, x, y - (i * spacing), scale, color, alpha, align, do_drop_shadow);
	}
}
