#pragma once

#include "core/types.h"

// Bindings to variables to their structs and stuff...
#define VAR_BINDINGS \
	/* :/Display */ \
	/* Currently empty, for the time being */ \
	\
	/* :/Audio */ \
	BIND(Audio, master_volume, vars->audio.master_volume, float) \
	BIND(Audio, music_volume, vars->audio.music_volume, float) \
	\
	/* :/Dev */ \
	BIND(Dev, console_small_openness, vars->dev.console_small_openness, float) \
	BIND(Dev, console_big_openness, vars->dev.console_big_openness, float) \
	BIND(Dev, console_openness_dt, vars->dev.console_openness_dt, float)
	
enum Section {
	SECTION_NONE = 0,
	
	SECTION_DISPLAY,
	SECTION_AUDIO,
	SECTION_DEV
};

struct Display {
	
};

struct Audio {
	float master_volume;
	float music_volume;
};

struct Dev {
	float console_small_openness;
	float console_big_openness;
	float console_openness_dt;
};

struct HotloadedVariables {
	Display display;
	Audio audio;
	Dev dev;
};

void init_vars(HotloadedVariables* vars, const char* path);
void reload_vars(HotloadedVariables* vars, const char* path);
