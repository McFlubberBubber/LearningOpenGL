#pragma once

#include <vector>
#include <string>

// Forward declarations
struct RenderingContext;

// These are the current menu options for the application
enum class MenuItem {
	RESUME,
	MUSIC,
	SCENE_SWITCH,
	QUIT,
	COUNT		// Extra enum type for cycling
};

struct Menu {
	MenuItem current_item = { MenuItem::RESUME };
	std::vector<MenuItem> items;
	
	bool do_music { true };				// By default, music will always be on.
	bool render_normal_scene{ true };	// Start with regular scene.
};


void init_menu(Menu* menu);
void draw_menu(RenderingContext* ctx, const Menu* menu);


// To allow cycling through MenuItem enum
void increment_menu_item(Menu* menu);
void decrement_menu_item(Menu* menu);
	
std::string menu_item_to_string(const Menu* menu, MenuItem item);
