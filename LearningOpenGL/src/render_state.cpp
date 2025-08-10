#include "render_state.h"
#include <glm/gtc/matrix_transform.hpp>


uint32_t RenderState::SCREEN_WIDTH  = 1600;
uint32_t RenderState::SCREEN_HEIGHT = 900;

float RenderState::ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);

glm::mat4 RenderState::ORTHO_PROJECTION = glm::ortho(0.0f, static_cast<float>(RenderState::SCREEN_WIDTH), 0.0f, static_cast<float>(RenderState::SCREEN_HEIGHT));


void RenderState::set_screen_size(uint32_t width, uint32_t height) {
	SCREEN_WIDTH  = width;
	SCREEN_HEIGHT = height;	

	ASPECT_RATIO = static_cast<float>(width) / static_cast<float>(height);
	update_ortho_projection();

	std::cout << "Width: " << width << " / Height: " << height << std::endl;
}

void RenderState::update_ortho_projection() {
	ORTHO_PROJECTION = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT);
}
