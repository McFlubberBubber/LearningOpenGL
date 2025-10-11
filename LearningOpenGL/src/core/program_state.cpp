#include "program_state.h"
#include "input/user_input.h"

#include <glm/gtc/matrix_transform.hpp>

bool init_application(ApplicationState *app, ViewportState *viewport) {
	std::cout << "OpenGL Version " << app->GL_MAJOR_VER<< "." << app->GL_MINOR_VER << "." 
		<< app->GL_BABY_VER << " - " << app->title << " by McFlubberBubber." << std::endl;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, app->GL_MAJOR_VER);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, app->GL_MINOR_VER);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	app->window = glfwCreateWindow(viewport->width, viewport->height, app->title, NULL, NULL);
	if (app->window == NULL)
	{
		std::cout << "Failed to load GLFW window!" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwSetWindowPos(app->window, 0, 40);
	glfwMakeContextCurrent(app->window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return false;
	}

	if (app->scene == SceneState::MAIN || app->scene == SceneState::SPACE) {
		glfwSetInputMode(app->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	// OpenGL Global Configuration	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);

	glfwSwapInterval(app->vsync);

	return true;
}


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
