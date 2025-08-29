#include "program_state.h"
#include <glm/gtc/matrix_transform.hpp>


void init_viewport(ViewportState* viewport) {
	viewport->width  = 1600;
	viewport->height = 900;
	
	viewport->aspect_ratio = static_cast<float>(viewport->width) / static_cast<float>(viewport->height);
	update_ortho(viewport);
}

void set_screen_size(ViewportState* viewport, u32 w, u32 h) {
	viewport->width  = w;
	viewport->height = h;
	
	viewport->aspect_ratio = static_cast<float>(w) / static_cast<float>(h);
	update_ortho(viewport);

	std::cout << "New screen size: " << w << "x" << h << std::endl;
}

void update_ortho(ViewportState* viewport) {
	viewport->ortho_projection = glm::ortho(0.0f, (float)(viewport->width),
											0.0f, (float)(viewport->height));
}
