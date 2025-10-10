#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "types.h"

// Game state
enum class SceneState {
	MAIN,
	MENU,
	SPACE
};

struct ApplicationState {
	SceneState scene   = SceneState::MAIN;
	GLFWwindow* window = nullptr;

	const char *title = "LearningOpenGL";

	u32 GL_MAJOR_VER = 4;
	u32 GL_MINOR_VER = 6;
	u32 GL_BABY_VER  = 0;

	bool fullscreen = false;
	bool vsync      = true;
};

struct ViewportState {
	glm::mat4 ortho_projection;
	u32 width  = 1600;
	u32 height = 900;
	
	float aspect_ratio = 16.0f/9.0f;
};

bool init_application(ApplicationState *app, ViewportState *viewport);
void init_viewport(ViewportState *viewport);
void set_screen_size(ViewportState *viewport, u32 w, u32 h);
void update_ortho(ViewportState *viewport);
