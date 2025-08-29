#include "debug_overlay.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "renderer/render_context.h"


void render_debug_overlay(RenderingContext* context, float dt) {
	// Drawing consistent UI
	display_fps(context, dt);
	display_coords(context);

	// @NOTE: This could be moved into the render_scene directly? But this
	// function is the only function currently that is rendering any 'interface'
	// to the screen so currently we will be updating the fading texts under this
	// function.
	update_and_draw_fading_texts(&context->assets.fonts[FONT_BOLD], context, dt);
}


// Pop up texts
void display_camera_mode_status(Assets* assets, CameraMode mode) {
	const std::string tag 	  = "camera_mode";
	const float x 			  = 100.0f;
	const float y 			  = 100.0f;
	const float scale 		  = 1.0f;
	const glm::vec3 color 	  = glm::vec3(1.0f);
	const float lifetime	  = 1.0f;
	const float fade_duration = 1.0f;
	const TextAlign align	  = TextAlign::LEFT;

	if (mode == CameraMode::FPS) 
		trigger_fading_text(&assets->fonts[FONT_BOLD], tag, "FPS Mode", x, y, scale, color, lifetime, fade_duration, align);
	else
		trigger_fading_text(&assets->fonts[FONT_BOLD], tag, "Freefly Mode", x, y, scale, color, lifetime, fade_duration, align);
}

void display_render_mode_status(Assets* assets, const ViewportState* viewport, RenderMode mode) {
	const std::string tag 	  = "render_mode";
	const float x			  = viewport->width / 2.0f;
	const float y			  = viewport->height / 1.5f;
	const float scale 		  = 1.0f;
	const glm::vec3 color 	  = glm::vec3(1.0f, 0.0f, 0.0f);
	const float lifetime	  = 1.0f;
	const float fade_duration = 1.0f;
	const TextAlign align	  = TextAlign::CENTER;

	trigger_fading_text(&assets->fonts[FONT_BOLD], tag, render_mode_to_string(mode), x, y, scale, color, lifetime, fade_duration, align);
}


// Consistent texts on the overlay
void display_fps(const RenderingContext* context, float dt) {
	const Assets* assets = &context->assets;
	
	static std::string fps_string;
	static float current_time = 0.0f;
	static float time_acc	  = 0.0f;
	static u32 counter	 	  = 0;

	const glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
	const float alpha = 1.0f;
	const float scale = 0.5f;
	const TextAlign align = TextAlign::LEFT;
	const bool do_drop_shadow = true;

	const float x = 0;
	const float y = (float)context->viewport.height - 25;

	//current_time = Time::get_time();
	counter++;

	time_acc += dt;
	if (time_acc >= 1.0f) {
		std::string fps = std::to_string(counter);
//		std::string ms = std::to_string(1000.0f / (float)counter);
		fps_string = fps + "FPS";
		counter = 0;
		time_acc = 0.0f;
	}
	
	draw_text(&assets->fonts[FONT_BOLD], context, fps_string, x, y, scale, color, alpha, align, do_drop_shadow);
}

void display_coords(const RenderingContext* context) {
	const Assets* assets = &context->assets;
	const Camera* camera = &context->camera_data.camera;

	static std::string x_str;
	static std::string y_str;
	static std::string z_str;

	const glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
	const float alpha = 1.0f;
	const float scale = 0.5f;
	const TextAlign align = TextAlign::LEFT;
	const bool do_drop_shadow = true;

	const float x = 0.0f;
	const float y = (float)context->viewport.height - 50;

	x_str = format_coord("X", (float)camera->position.x);
	y_str = format_coord("Y", (float)camera->position.y);
	z_str = format_coord("Z", (float)camera->position.z);

	draw_text(&assets->fonts[FONT_BOLD], context, x_str, x, y, scale, color, alpha, align, do_drop_shadow);
	
	draw_text(&assets->fonts[FONT_BOLD], context, y_str, (x + 110), y, scale, color, alpha, align, do_drop_shadow);
	
	draw_text(&assets->fonts[FONT_BOLD], context, z_str, (x + 220), y, scale, color, alpha, align, do_drop_shadow);
}


// Internal helpers
// Rendering world coordinates (with rounded floats to 2dp)
std::string format_coord(const std::string& label, float value){
	std::ostringstream oss;
	oss << label << ": " << std::fixed << std::setw(6) << std::setprecision(2) << value;
	return oss.str();
}

std::string render_mode_to_string(RenderMode render_mode) {
	switch (render_mode) {
		case RenderMode::NORMAL:			return "NORMAL";
		case RenderMode::INVERT:			return "INVERT";
		case RenderMode::GRAYSCALE:			return "GRAYSCALE";
		case RenderMode::SHARPEN:			return "SHARPEN";
		case RenderMode::DARK_SHARPEN:		return "DARK SHARPEN";
		default:							return "ERROR!";
	}
}
