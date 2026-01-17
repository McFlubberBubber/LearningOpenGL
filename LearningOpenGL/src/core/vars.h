#pragma once

#include "core/types.h"

// Forward declarations.
struct Console;

// Bindings to variables to their structs and stuff...
#define VAR_BINDINGS \
	/* :/Display */ \
	/* Currently empty, for the time being */ \
	\
	/* :/Audio */ \
	BIND(Audio, master_volume, vars->audio.master_volume, float) \
	BIND(Audio, music_volume, vars->audio.music_volume, float) \
	\
	/* :/Scene */ \
	BIND(Scene, shadow_light_color_x, vars->scene.shadow_light_color_x, float) \
	BIND(Scene, shadow_light_color_y, vars->scene.shadow_light_color_y, float) \
	BIND(Scene, shadow_light_color_z, vars->scene.shadow_light_color_z, float) \
	/* :/Dev */ \
	BIND(Dev, console_small_openness, vars->dev.console_small_openness, float) \
	BIND(Dev, console_big_openness, vars->dev.console_big_openness, float) \
	BIND(Dev, console_openness_dt, vars->dev.console_openness_dt, float)
	
enum Section {
	SECTION_NONE = 0,
	
	SECTION_DISPLAY,
	SECTION_AUDIO,
	SECTION_DEV,
	SECTION_SCENE
};

struct Display {
	
};

struct Audio {
	float master_volume;
	float music_volume;
};

struct Scene {
	float shadow_light_color_x;
	float shadow_light_color_y;
	float shadow_light_color_z;
};


struct Dev {
	float console_small_openness;
	float console_big_openness;
	float console_openness_dt;
};

struct HotloadedVariables {
	const char* file_path;
	Console* console_ptr; // This is here for access to console logging.

	Display display;
	Audio audio;
	Scene scene;
	Dev dev;
};

void init_vars(HotloadedVariables* vars, const char* path, Console* console);
void reload_vars(HotloadedVariables* vars);

bool write_to_vars(HotloadedVariables* vars, const std::vector<std::string>& tokens);
std::vector<std::string> get_all_lines(HotloadedVariables* vars);
