#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>


struct RenderState {
	static uint32_t SCREEN_WIDTH;
	static uint32_t SCREEN_HEIGHT;
	static float ASPECT_RATIO;

	static glm::mat4 ORTHO_PROJECTION;

	static void set_screen_size(uint32_t width, uint32_t height);
	static void update_ortho_projection();
	
};	
