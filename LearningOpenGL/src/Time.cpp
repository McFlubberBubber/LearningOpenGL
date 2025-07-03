#include "Time.h"
#include <GLFW/glfw3.h>

namespace Time {
	static float lastFrame = 0.0f;
	static float deltaTime = 0.0f;

	// Updating the time variables each render frame
	void update() {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	// Getters for time variables
	float getDeltaTime() {
		return deltaTime;
	}

	float getTime() {
		return static_cast<float>(glfwGetTime());
	}
}