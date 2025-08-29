#pragma once

#include <vector>
#include <string>

// Forward declarations
struct RenderingContext;

// These are the current menu options for the application
enum class MenuItem {
	RESUME,
	MUSIC,
	QUIT,
	COUNT		// Extra enum type for cycling
};

struct Menu {
	MenuItem current_item;
	std::vector<MenuItem> items;
	
	bool do_music { true };		// By default, music will always be on
};


void init_menu(Menu* menu);
void draw_menu(const RenderingContext* context, const Menu* menu);


// To allow cycling through MenuItem enum
void increment_menu_item(Menu* menu);
void decrement_menu_item(Menu* menu);
	
std::string menu_item_to_string(const Menu* menu, MenuItem item);
