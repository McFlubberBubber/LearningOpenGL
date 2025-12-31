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

void init_menu(Menu* menu, ConfigFile *cfg) {
	menu->main.items = {
		MenuItem::RESUME,
		MenuItem::OPTIONS,
		MenuItem::SCENE_SWITCH,
		MenuItem::QUIT
	};

	menu->options.items = {
		OptionsItem::MUSIC,
		OptionsItem::DISPLAY,
		OptionsItem::VSYNC,
		
		OptionsItem::GAMMA,
		OptionsItem::MULTISAMPLING,

		OptionsItem::BACK
	};

	// Fetching data from the config file.
	menu->do_music		= cfg->music;
	menu->do_fullscreen = cfg->fullscreen;
	menu->do_vsync		= cfg->vsync;

	menu->gamma			   = cfg->gamma;
	menu->do_multisampling = cfg->multisampling;
	
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

	if (ctx->app.config.debug_mode)
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

void handle_main_menu_activations(Menu* menu, RenderingContext* ctx) {
	switch (menu->main.current_item) {
	case MenuItem::RESUME: {
		ctx->app.scene = ctx->app.prev_scene;
		break;
	}
		
	case MenuItem::OPTIONS: {
		menu->current_page = MenuPage::OPTIONS;
		menu->options.current_item = OptionsItem::MUSIC; // Resetting
		break;
	}
		
	case MenuItem::SCENE_SWITCH: {
		menu->render_normal_scene = !menu->render_normal_scene;
		if (menu->render_normal_scene) {
			ctx->app.scene = SceneState::MAIN;
			display_current_scene_status(ctx);
		} else {
			ctx->app.scene = SceneState::SPACE;
			display_current_scene_status(ctx);
		}
		break;
	}
		
	case MenuItem::QUIT: {
		glfwSetWindowShouldClose(ctx->app.window, true);
		ctx->app.is_running = false;
		break;
	}
		
	default: {
		break;
	}
		
	}
}


void handle_options_menu_activations(Menu* menu, RenderingContext* ctx) {
	std::string message;
	
	switch(menu->options.current_item) {
	case OptionsItem::DISPLAY: {
		ctx->app.config.fullscreen = !ctx->app.config.fullscreen;
		menu->do_fullscreen = ctx->app.config.fullscreen;
		message = std::string("Changed display to ") + (menu->do_fullscreen ? "fullscreen" : "windowed") + " mode.";
		push_message(&ctx->message_queue, message);
		break;
	}
		
	case OptionsItem::MUSIC: {
		menu->do_music = !menu->do_music;
		message = std::string("Music toggled to ") + (menu->do_music ? "ON" : "OFF") + " state.";
		push_message(&ctx->message_queue, message);
		break;
	}
		
	case OptionsItem::VSYNC: {
		ctx->app.config.vsync = !ctx->app.config.vsync;
		menu->do_vsync = ctx->app.config.vsync;
		message = std::string("Vsync ") + (ctx->app.config.vsync ? "Enabled!" : "Disabled!");
		push_message(&ctx->message_queue, message);
		break;
	}

	// @TODO: Update screen shader to use a uniform to read gamma flags +
	// clamp values from 0.1f to 1.0f (gamma goes from 0.1 to 2.2)
	case OptionsItem::GAMMA: {
		// @TODO: Use arrow keys to handle slider?
		push_message(&ctx->message_queue, "Gamma slider not implemented!");
		break;
	}

	// @TODO: Fix the buffers in BufferData to implement this.
	case OptionsItem::MULTISAMPLING: {
		menu->do_multisampling = !menu->do_multisampling;
		message = std::string("Multisampling toggle not implemented!");
		push_message(&ctx->message_queue, message);
		break;
	}
		
	case OptionsItem::BACK: {
		menu->current_page = MenuPage::MAIN;
		menu->main.current_item = MenuItem::RESUME; // Resetting
		break;
	}
		
	default: {
		break;
	}
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
	case::OptionsItem::DISPLAY:
		return std::string("Display: ") + (menu->do_fullscreen ? "Fullscreen" : "Windowed");

	case OptionsItem::MUSIC:
		return std::string("Music: ") + (menu->do_music ? "ON" : "OFF");

	case::OptionsItem::VSYNC:
		return std::string("V-Sync: ") + (menu->do_vsync ? "Enabled" : "Disabled");
		
	// @TODO: These gamma values may need to get adjusted from a simple 0-100%
	case OptionsItem::GAMMA:
		oss << "Gamma: " << std::fixed << std::setw(6) << std::setprecision(2) << menu->gamma;
		return oss.str();

	case OptionsItem::MULTISAMPLING:
		return std::string("Multisampling: ") + (menu->do_multisampling ? "Enabled" : "Disabled");
		
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
	float y		  = vp->height / 1.5f;

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
