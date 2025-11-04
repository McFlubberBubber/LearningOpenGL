#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "types.h"

struct ConfigFile {
	// Purely for storing the configuration file path.
	std::string path;

	// The rest of the data beneath is in regards to the config.ini file that has
	// the sections and key + values assigned like so.

	// Display
	bool fullscreen;
	bool vsync;
	bool multisampling;
	float gamma;
	s32 width;
	s32 height;

	// Audio
	bool music;
};

// Game state
enum class SceneState {
	MAIN,
	MENU,
	SPACE
};

struct ApplicationState {
	SceneState scene   = SceneState::MAIN;

	GLFWmonitor* monitor = nullptr;
	GLFWwindow* window   = nullptr;	

	const char *title = "LearningOpenGL";

	u32 GL_MAJOR_VER = 4;
	u32 GL_MINOR_VER = 6;
	u32 GL_BABY_VER  = 0;

	u32 sample_count = 4;

	s32 windowed_xpos	= 0;
	s32 windowed_ypos   = 30;
	s32 windowed_width  = 1600;
	s32 windowed_height = 900;

	ConfigFile config = {};
};

struct ViewportState {
	glm::mat4 ortho_projection;
	u32 width  = 1600;
	u32 height = 900;
	
	float aspect_ratio = 16.0f/9.0f;
};

bool init_application(ApplicationState *app, ViewportState *viewport);
void set_viewport(ViewportState *viewport, ApplicationState *app);

void set_screen_size(ViewportState *viewport, u32 w, u32 h);
void update_ortho(ViewportState *viewport);
void check_for_window_updates(ApplicationState *app, ViewportState *vp);

std::string get_executable_directory();
void load_config_file(ConfigFile* cfg);
void update_config_from_app(const ApplicationState* app);
