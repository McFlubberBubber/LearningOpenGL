#include "debug_overlay.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "renderer/render_context.h"


void render_debug_overlay(RenderingContext* ctx, float dt) {
	// Drawing consistent UI
	display_fps(ctx, dt);
	display_coords(ctx);
	display_euler_angles(ctx);

	// @NOTE: This could be moved into the render_scene directly? But this
	// function is the only function currently that is rendering any 'interface'
	// to the screen so currently we will be updating the fading texts under this
	// function.
	update_and_draw_fading_texts(&ctx->assets.fonts[FONT_BOLD], ctx, dt);
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

void display_zoom(Assets* assets, const ViewportState* viewport, const CameraData* cd) {
	const std::string tag 	  = "zoom_status";
	const float x 			  = viewport->width / 2.0f;
	const float y 			  = 100.0f;
	const float scale 		  = 1.0f;
	const glm::vec3 color 	  = glm::vec3(1.0f);
	const float lifetime	  = 1.0f;
	const float fade_duration = 1.0f;
	const TextAlign align	  = TextAlign::CENTER;

	std::string zoom_text = "Zoom: ";
	u32 zoom_value = (u32)cd->camera.zoom;
	zoom_text += std::to_string(zoom_value);

	trigger_fading_text(&assets->fonts[FONT_BOLD], tag, zoom_text, x, y, scale, color, lifetime, fade_duration, align);
}


// Consistent texts on the overlay
void display_fps(const RenderingContext* ctx, float dt) {
	const Assets* assets = &ctx->assets;
	
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
	const float y = (float)ctx->viewport.height - 25;

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
	
	draw_text(&assets->fonts[FONT_BOLD], ctx, fps_string, x, y, scale, color, alpha, align, do_drop_shadow);
}

void display_coords(const RenderingContext* ctx) {
	const Assets* assets = &ctx->assets;
	const Camera* camera = &ctx->camera_data.camera;

	static std::string x_str;
	static std::string y_str;
	static std::string z_str;

	const glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
	const float alpha = 1.0f;
	const float scale = 0.5f;
	const TextAlign align = TextAlign::LEFT;
	const bool do_drop_shadow = true;

	const float x = 0.0f;
	const float y = (float)ctx->viewport.height - 50;

	x_str = format_coord("X", (float)camera->position.x);
	y_str = format_coord("Y", (float)camera->position.y);
	z_str = format_coord("Z", (float)camera->position.z);

	draw_text(&assets->fonts[FONT_BOLD], ctx, x_str, x, y, scale, color, alpha, align, do_drop_shadow);
	
	draw_text(&assets->fonts[FONT_BOLD], ctx, y_str, (x + 110), y, scale, color, alpha, align, do_drop_shadow);
	
	draw_text(&assets->fonts[FONT_BOLD], ctx, z_str, (x + 220), y, scale, color, alpha, align, do_drop_shadow);
}

void display_euler_angles(const RenderingContext* ctx) {
	const Assets* assets = &ctx->assets;
	const Camera* camera = &ctx->camera_data.camera;

	static std::string yaw_text;
	static std::string pitch_text;

	const glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
	const float alpha = 1.0f;
	const float scale = 0.5f;
	const TextAlign align = TextAlign::LEFT;
	const bool do_drop_shadow = true;

	const float y = (float)ctx->viewport.height - 75;
	
	yaw_text = format_coord("Yaw: ", (float)camera->yaw);
	pitch_text = format_coord("Pitch: ", (float)camera->pitch);
	
	draw_text(&assets->fonts[FONT_BOLD], ctx, yaw_text, 0, y, scale, color, alpha, align, do_drop_shadow);

	draw_text(&assets->fonts[FONT_BOLD], ctx, pitch_text, 0, y - 25, scale, color, alpha, align, do_drop_shadow);
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
