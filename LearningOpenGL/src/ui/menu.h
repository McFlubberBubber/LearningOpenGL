#pragma once

#include <vector>
#include <string>

// Forward declarations
struct RenderingContext;

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

	// Global settings.
	float gamma 		  	 { 1.0f };
	bool do_music			 { true };
	bool render_normal_scene { true };
	bool do_multisampling 	 { true };
};

void init_menu(Menu* menu);
void draw_menu(RenderingContext* ctx, Menu* menu);
void increment_menu_item(Menu* menu);
void decrement_menu_item(Menu* menu);
	
