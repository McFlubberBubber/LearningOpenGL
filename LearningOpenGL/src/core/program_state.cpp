#include "program_state.h"
#include "input/user_input.h"
#include "inicpp.h"

#include <iostream>
#include <windows.h> // To fetch the .exe path
#include <glm/gtc/matrix_transform.hpp>

void set_viewport(ViewportState* viewport, ApplicationState *app) {
	if (app->config.fullscreen) {
		viewport->width  = app->config.fullscreen_width;
		viewport->height = app->config.fullscreen_height;
	} else {
		viewport->width  = app->config.width;
		viewport->height = app->config.height;
	}
	
	viewport->aspect_ratio = static_cast<float>(viewport->width) / static_cast<float>(viewport->height);
	update_ortho(viewport);
}

bool init_application(ApplicationState *app, ViewportState *viewport) {
	std::cout << "OpenGL Version " << app->GL_MAJOR_VER<< "." << app->GL_MINOR_VER << "." << app->GL_BABY_VER << " - " << app->title << " by McFlubberBubber." << std::endl;

	// Creating a temp instance of the user config.
	load_config_file(&app->config);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, app->GL_MAJOR_VER);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, app->GL_MINOR_VER);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Prevents custom resizing.

	app->is_running = true;

	// OPTIONAL: Built in multisampling from OpenGL.
	// glfwWindowHint(GLFW_SAMPLES, app->sample_count); // Multisampling

	// Getting the primary monitor
	app->monitor = glfwGetPrimaryMonitor();
	if (app->monitor == NULL) {
		std::cout << "Failed to fetch primary monitor!" << std::endl;
		glfwTerminate();
		return false;
	}

	// Fetching the monitor's dimensions.
	const GLFWvidmode* mode = glfwGetVideoMode(app->monitor);
	if (mode == NULL) {
		std::cout << "Failed to fetch video mode!" << std::endl;
		glfwTerminate();
		return false;
	}
	app->config.fullscreen_width  = mode->width;
	app->config.fullscreen_height = mode->height;

	// Window creation
	if (app->config.fullscreen) {
		app->window = glfwCreateWindow(app->config.fullscreen_width, app->config.fullscreen_height, app->title, app->monitor, NULL);
	} else {
		app->window = glfwCreateWindow(app->config.width, app->config.height, app->title, NULL, NULL);
		if (app->window == NULL) {
			std::cout << "Failed to load GLFW window!" << std::endl;
			glfwTerminate();
			return false;
		}
	}

	set_viewport(viewport, app);
	glfwSetWindowPos(app->window, 0, 30);
	glfwMakeContextCurrent(app->window);

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

	if (app->config.multisampling)
		glEnable(GL_MULTISAMPLE);

	return true;
}

void set_screen_size(ViewportState* viewport, ApplicationState* app, u32 w, u32 h) {
	viewport->width  = w;
	viewport->height = h;
	
	viewport->aspect_ratio = static_cast<float>(w) / static_cast<float>(h);
	update_ortho(viewport);

	// @NOTE: Currently, the only way to change the screen size dimensions is when
	// we are in windowed mode, there is yet to be a fullscreen resolution feature to 
	// allow changing it, therefore we always set the app configs' regular width and
	// height to whatever parameters we get here.
	app->config.width  = w;
	app->config.height = h;

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
	// Check if the window must close from GLFW.
	if (glfwWindowShouldClose(app->window)) {
		app->is_running = false;
	}

	// V-sync updating
	glfwSwapInterval(app->config.vsync ? 1 : 0);

	// Detecting display updates
	static bool prev_display_state = app->config.fullscreen;
	
	if (app->config.fullscreen != prev_display_state) {

		// Switching to fullscreen mode
		if (app->config.fullscreen) {
			// Save the data before going fullscreen
			glfwGetWindowPos(app->window, &app->windowed_xpos, &app->windowed_ypos);
			glfwGetWindowSize(app->window, &app->config.width, &app->config.height);

			// Fetching the refresh rate
			const GLFWvidmode* mode = glfwGetVideoMode(app->monitor);
			int refresh_rate = mode->refreshRate;

			// Setting fullscreen dimensions.
			glfwSetWindowMonitor(app->window, app->monitor, 0, 0,
								 app->config.fullscreen_width, app->config.fullscreen_height, refresh_rate);
			vp->width  = app->config.fullscreen_width;
			vp->height = app->config.fullscreen_height;

		// Switching to windowed mode		
		} else {
			glfwSetWindowMonitor(app->window, NULL, app->windowed_xpos, app->windowed_ypos, app->config.width, app->config.height, GLFW_DONT_CARE);
			vp->width  = app->config.width;
			vp->height = app->config.height;
		}

		prev_display_state = app->config.fullscreen;		
	}
}

// @WARNING: This is Windows only code, other OS' need different functionality here.
std::string get_executable_directory() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	// Find the last backslash to get directory path
	std::string exePath(buffer);
	size_t lastSlash = exePath.find_last_of("\\/");

	if (lastSlash != std::string::npos) {
		return exePath.substr(0, lastSlash);
	}

	return ""; // Fallback if something went wrong
}

static void print_cfg(const ConfigFile* cfg) {
	// Display
	std::cout << "[Display]" << std::endl;
	std::cout << "Fullscreen    = " << cfg->fullscreen << std::endl;
	std::cout << "V-Sync        = " << cfg->vsync << std::endl;
	std::cout << "Multisampling = " << cfg->multisampling << std::endl;
	std::cout << "Gamma         = " << cfg->gamma << std::endl;
	std::cout << "Screen Width  = " << cfg->fullscreen_width << std::endl;
	std::cout << "Screen Height = " << cfg->fullscreen_height << std::endl;
	std::cout << "Width         = " << cfg->width << std::endl;
	std::cout << "Height        = " << cfg->height << std::endl;

	// Audio
	std::cout << "[Audio]" << std::endl;
	std::cout << "Music state   = " << cfg->music << std::endl;

	// Dev
	std::cout << "[Dev]" << std::endl;
	std::cout << "Debug mode    = " << cfg->debug_mode << std::endl;
}

// Initially reading the .ini file to apply to the config.ini
void load_config_file(ConfigFile* cfg) {
	using namespace ini;

	// Getting the file path, then loading the ini.
	cfg->path = get_executable_directory() + "\\config.ini";
	IniFile ini;
	ini.load(cfg->path);
	assert(cfg->path != "");
	
	// @TODO: Switch to a better .ini parser because this loop magically fixes some messed up bug,
	// where basically, if we don't have a config.ini file already in the output directory,
	// this code will somehow fail and the reading of the first attribute, will actually result
	// in a data error. Therefore, we should either leave this code in, or just switch to a better
	// library which looks to be a better option because wtf.
	//				- nathan, 04 Nov 2025

	for (const auto& sectionPair : ini) {
		// const std::string& sectionName = sectionPair.first;
		// const IniSection& section = sectionPair.second;

		for (const auto& fieldPair : sectionPair.second) {
			// const std::string& fieldName = fieldPair.first;
			// const IniField& field = fieldPair.second;
		}
	}
	
	// Setting display stuff
	cfg->fullscreen	 		= ini["Display"]["fullscreen"].as<bool>();
	cfg->vsync		  		= ini["Display"]["vsync"].as<bool>();
	cfg->multisampling		= ini["Display"]["multisampling"].as<bool>();
	cfg->gamma		   		= ini["Display"]["gamma"].as<float>();
	cfg->fullscreen_width   = ini["Display"]["fullscreen_width"].as<int>();
	cfg->fullscreen_height  = ini["Display"]["fullscreen_height"].as<int>();
	cfg->width		        = ini["Display"]["width"].as<int>();
	cfg->height		   		= ini["Display"]["height"].as<int>();

	// Setting audio stuff
	cfg->music = ini["Audio"]["music"].as<bool>();

	// Setting debug-related stuff
	cfg->debug_mode = ini["Dev"]["debug_mode"].as<bool>();

	std::cout << "Loading config file at path: " << cfg->path << std::endl;
	print_cfg(cfg);
}

// Called right after the main loop is finished to update the configuration.
void update_config_from_app(const ApplicationState* app) {
	using namespace ini;

	IniFile ini;
	ini.load(app->config.path);
	
	// [Display]
	ini["Display"]["fullscreen"] = app->config.fullscreen;
	ini["Display"]["vsync"] = app->config.vsync;
	ini["Display"]["multisampling"] = app->config.multisampling;
	ini["Display"]["gamma"] = app->config.gamma;
	ini["Display"]["fullscreen_width"] = app->config.fullscreen_width;
	ini["Display"]["fullscreen_height"] = app->config.fullscreen_height;
	ini["Display"]["width"] = app->config.width;
	ini["Display"]["height"] = app->config.height;

	// [Audio]
	ini["Audio"]["music"] = app->config.music;

	// [Dev]
	ini["Dev"]["debug_mode"] = app->config.debug_mode;

	ini.save(app->config.path);
	std::cout << "Updated config file at path: " << app->config.path << std::endl;
	print_cfg(&app->config);
}
