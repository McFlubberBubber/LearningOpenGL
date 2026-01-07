#pragma once

#include <vector>
#include <string>

// Forward declarations
struct RenderingContext;
struct ConfigFile;

enum class MenuPage {
	MAIN,
	OPTIONS
};

enum class MenuItem {
	RESUME,
	OPTIONS,
	SCENE_SWITCH,
	QUIT,

	COUNT		// Extra enum type for cycling
};

enum class OptionsItem {
	MUSIC,
	DISPLAY,
	VSYNC,

	GAMMA,
	MULTISAMPLING,

	BACK,

	COUNT
};

struct MainMenuState {
	MenuItem current_item = MenuItem::RESUME;
	std::vector<MenuItem> items;
};

struct OptionsState {
	OptionsItem current_item = OptionsItem::MUSIC;
	std::vector<OptionsItem> items;
};

struct Menu {
	MenuPage current_page = MenuPage::MAIN;
	MainMenuState main;
	OptionsState options;

	// @TODO: These settings should be read from the config file.
	RenderingContext* render_ctx_ptr = NULL;

	bool do_music;
	bool do_fullscreen;
	bool do_vsync; 

	float gamma;
	bool do_multisampling; 
};

void init_menu(Menu* menu, RenderingContext* ctx);
void draw_menu(Menu* menu, RenderingContext* ctx);

void increment_menu_item(Menu* menu);
void decrement_menu_item(Menu* menu);

void handle_main_menu_activations(Menu* menu, RenderingContext*);
void handle_options_menu_activations(Menu* menu, RenderingContext* ctx);
	
