#include "program_state.h"
#include "input/user_input.h"

#include <glm/gtc/matrix_transform.hpp>

// NOTE: We init_viewport() first since that sets the width and height of the
// window which is what the glfwWindow requires when being initialized.

void init_viewport(ViewportState* viewport) {
	viewport->width  = 1600;
	viewport->height = 900;
	
	viewport->aspect_ratio = static_cast<float>(viewport->width) / static_cast<float>(viewport->height);
	update_ortho(viewport);
}

bool init_application(ApplicationState *app, ViewportState *viewport) {
	std::cout << "OpenGL Version " << app->GL_MAJOR_VER<< "." << app->GL_MINOR_VER << "." << app->GL_BABY_VER << " - " << app->title << " by McFlubberBubber." << std::endl;

	// Initializing app data
	app->windowed_width  = viewport->width;
	app->windowed_height = viewport->height;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, app->GL_MAJOR_VER);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, app->GL_MINOR_VER);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// OPTIONAL: Built in multisampling from OpenGL.
	// glfwWindowHint(GLFW_SAMPLES, app->sample_count); // Multisampling

	// Window creation
	app->window = glfwCreateWindow(app->windowed_width, app->windowed_height, app->title, NULL, NULL);
	if (app->window == NULL) {
		std::cout << "Failed to load GLFW window!" << std::endl;
		glfwTerminate();
		return false;
	}
	
	glfwSetWindowPos(app->window, 0, 30);
	glfwMakeContextCurrent(app->window);

	// Getting the primary monitor
	app->monitor = glfwGetPrimaryMonitor();
	if (app->monitor == NULL) {
		std::cout << "Failed to fetch primary monitor!" << std::endl;
		glfwTerminate();
		return false;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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

	if (app->multisampling)
		glEnable(GL_MULTISAMPLE);

	return true;
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

// @TODO: This function being called every frame is probably not needed
// since we are only really checking for updates when the user toggles them, so
// listening in for window updates every frame is useless.

// Understanding which things are events prevents us from needing poll operations
// like this which can eat into performance. But hey it works for now.
//			-nathan, 26th Oct 2025

void check_for_window_updates(ApplicationState* app, ViewportState* vp) {
	// V-sync updating
	glfwSwapInterval(app->vsync ? 1 : 0);

	// Detecting display updates
	static bool prev_display_state = app->fullscreen;
	
	if (app->fullscreen != prev_display_state) {
		// Switching to fullscreen mode
		if (app->fullscreen) {
			// Save the data before going fullscreen
			glfwGetWindowPos(app->window, &app->windowed_xpos, &app->windowed_ypos);
			glfwGetWindowSize(app->window, &app->windowed_width, &app->windowed_height);

			const GLFWvidmode* mode = glfwGetVideoMode(app->monitor);
			int refresh_rate 		= mode->refreshRate;

			// Setting fullscreen dimensions (always 1920x1080 for now).
			glfwSetWindowMonitor(app->window, app->monitor, 0, 0, 1920, 1080, refresh_rate);
			vp->width  = 1920;
			vp->height = 1080;

		// Switching to windowed mode		
		} else {
			glfwSetWindowMonitor(app->window, NULL, app->windowed_xpos, app->windowed_ypos, app->windowed_width, app->windowed_height, GLFW_DONT_CARE);
			vp->width  = app->windowed_width;
			vp->height = app->windowed_height;
		}

		prev_display_state = app->fullscreen;		
	}
}
