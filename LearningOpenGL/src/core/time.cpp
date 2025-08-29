#include "time.h"
#include <GLFW/glfw3.h>

namespace Time {
	static float last_frame = 0.0f;
	static float delta_time = 0.0f;

	// Updating the time variables each render frame
	void update() {
		float current_frame = static_cast<float>(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
	}

	// Getters for time variables
	float get_delta_time() {
		return delta_time;
	}

	float get_time() {
		return static_cast<float>(glfwGetTime());
	}
}
