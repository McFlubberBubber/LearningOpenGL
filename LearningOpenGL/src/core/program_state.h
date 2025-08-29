#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "types.h"

// Game state
enum class ApplicationState {
	GAME,
	MENU
};

struct ViewportState {
	glm::mat4 ortho_projection;
	u32 width  = 1600;
	u32 height = 900;
	
	float aspect_ratio = 16.0f/9.0f;

	bool fullscreen = false;
	bool vsync 		= true;
};

void init_viewport(ViewportState* viewport);
void set_screen_size(ViewportState* viewport, u32 w, u32 h);
void update_ortho(ViewportState* viewport);
