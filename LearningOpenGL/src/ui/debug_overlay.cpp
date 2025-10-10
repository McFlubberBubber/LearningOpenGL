#include "debug_overlay.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "renderer/render_context.h"
#include "core/program_state.h"

namespace DebugOverlay {
	constexpr float SCALE = 1.0f;
	constexpr float ALPHA = 1.0f;

	constexpr float LIFETIME 	  = 1.0f;
	constexpr float FADE_DURATION = 1.0f;
	constexpr bool DO_DROP_SHADOW = true;

	// GLM stuff doesn't have much constexpr support in this version.
	const glm::vec3 GREEN_COLOR   {0.0f, 1.0f, 0.0f};
	const glm::vec3 WHITE_COLOR   {1.0f, 1.0f, 1.0f};
	const glm::vec3 RED_COLOR     {1.0f, 0.0f, 0.0f};
	const glm::vec3 GOLDISH_COLOR {0.95f, 0.95f, 0.8f};
	const glm::vec3 BLACK_COLOR   {0.1f, 0.1f, 0.1f};

	constexpr float LINE_HEIGHT = 25.0f;
	constexpr float COORD_SPACING = 110.0f;
}


void render_debug_overlay(RenderingContext* ctx, float dt) {
	// Drawing consistent UI based on the debug_mode flag

	if (ctx->debug_mode) {
		display_fps(ctx, dt);
		display_coords(ctx);
		display_euler_angles(ctx);
		display_sprint_status(ctx);
	}

	// @NOTE: This could be moved into the render_scene directly? But this
	// function is the only function currently that is rendering any 'interface'
	// to the screen so currently we will be updating the fading texts under this
	// function.
	update_and_draw_fading_texts(&ctx->assets.fonts[FONT_BOLD], ctx, dt);
	update_and_draw_text_boxes(&ctx->assets.fonts[FONT_BOLD], ctx, dt);
	update_and_draw_message_queue(&ctx->assets.fonts[FONT_BOLD], ctx, dt);
}


// Pop up texts
// @TODO: These hard coded std::string tags are pretty bad, I'd like to move these into a specific enum that we can just specify here.
void display_camera_mode_status(Assets* assets, CameraMode mode) {
	using namespace DebugOverlay;
	const FontTag tag		  = FontTag::TAG_CAMERA;
	const float x 			  = 100.0f;
	const float y 			  = 100.0f;
	const TextAlign align	  = TextAlign::LEFT;

	if (mode == CameraMode::FPS) 
		trigger_fading_text(&assets->fonts[FONT_BOLD], tag, "FPS Mode", x, y, SCALE, WHITE_COLOR, LIFETIME, FADE_DURATION, align);
	else
		trigger_fading_text(&assets->fonts[FONT_BOLD], tag, "Freefly Mode", x, y, SCALE, WHITE_COLOR, LIFETIME, FADE_DURATION, align);
}

void display_render_mode_status(RenderingContext* ctx) {
	using namespace DebugOverlay;
	auto viewport = &ctx->viewport;
	auto assets   = &ctx->assets;
	auto mode	  = ctx->post_processing.mode;

	const FontTag tag		  = FontTag::TAG_RENDER;
	const float x			  = viewport->width / 2.0f;
	const float y			  = viewport->height / 1.5f;
	const TextAlign align	  = TextAlign::CENTER;

//	trigger_fading_text(&assets->fonts[FONT_BOLD], tag, render_mode_to_string(mode), x, y, SCALE, RED_COLOR, LIFETIME, FADE_DURATION, align);
//	trigger_text_box(&assets->fonts[FONT_BOLD], tag, render_mode_to_string(mode), x, y, SCALE, RED_COLOR, BLACK_COLOR, LIFETIME, FADE_DURATION, align);
	const std::string base = "RenderMode: ";
	std::string text = base + render_mode_to_string(mode);
	
	push_message(&ctx->message_queue, text);



}

void display_debug_mode_status(RenderingContext* ctx) {
	using namespace DebugOverlay;
	const FontTag tag	  = FontTag::TAG_DEBUG;
	const float x		  = ctx->viewport.width  / 2.0f;
	const float y		  = ctx->viewport.height / 1.25f;
	const TextAlign align = TextAlign::CENTER;

	const std::string base = "Debug mode ";
	std::string text = "";
	if (ctx->debug_mode == false) {
		text = base + "disabled";
	} else {
		text = base + "enabled";
	}

//	trigger_text_box(&ctx->assets.fonts[FONT_BOLD], tag, text, x, y, SCALE, GOLDISH_COLOR, BLACK_COLOR, LIFETIME, FADE_DURATION,align);
	
	push_message(&ctx->message_queue, text);
}

void display_current_scene_status(RenderingContext* ctx) {
	using namespace DebugOverlay;
	std::string text = {};
	if (ctx->app.scene == SceneState::MAIN) {
		text = "Rendering normal scene";
		push_message(&ctx->message_queue, text);
	}
	if (ctx->app.scene == SceneState::SPACE) {
		text = "Rendering space scene";
		push_message(&ctx->message_queue, text);
	}
}


void display_zoom(Assets* assets, const ViewportState* viewport, const CameraData* cd) {
	using namespace DebugOverlay;
	const FontTag tag		  = FontTag::TAG_ZOOM;
	const float x 			  = viewport->width / 2.0f;
	const float y 			  = 100.0f;
	const TextAlign align	  = TextAlign::CENTER;

	std::string zoom_text = "Zoom: ";
	u32 zoom_value = (u32)cd->camera.zoom;
	zoom_text += std::to_string(zoom_value);

	trigger_fading_text(&assets->fonts[FONT_BOLD], tag, zoom_text, x, y, SCALE, WHITE_COLOR, LIFETIME, FADE_DURATION, align);
}


// Consistent texts on the overlay
void display_fps(const RenderingContext* ctx, float dt) {
	using namespace DebugOverlay;
	const Font* bold_font = &ctx->assets.fonts[FONT_BOLD];
	
	static std::string fps_string;
	static float current_time = 0.0f;
	static float time_acc	  = 0.0f;
	static u32 counter	 	  = 0;

	const float scale = 0.5f;
	const TextAlign align = TextAlign::LEFT;
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
	
	draw_text(bold_font, ctx, fps_string, x, y, scale, GREEN_COLOR, ALPHA, align, DO_DROP_SHADOW);
}

void display_coords(const RenderingContext* ctx) {
	using namespace DebugOverlay;
	const Font* bold_font = &ctx->assets.fonts[FONT_BOLD];
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

	draw_text(bold_font, ctx, x_str, x, y, scale, GREEN_COLOR, ALPHA, align, DO_DROP_SHADOW);
	draw_text(bold_font, ctx, y_str, (x + COORD_SPACING), y, scale, GREEN_COLOR, ALPHA, align, DO_DROP_SHADOW);
	draw_text(bold_font, ctx, z_str, (x + COORD_SPACING * 2), y, scale, GREEN_COLOR, ALPHA, align, DO_DROP_SHADOW);
}

void display_euler_angles(const RenderingContext* ctx) {
	using namespace DebugOverlay;
	const Font* bold_font = &ctx->assets.fonts[FONT_BOLD];
	const Camera* camera = &ctx->camera_data.camera;

	const float scale = 0.5f;
	const TextAlign align = TextAlign::LEFT;

	const float x = 0;
	const float y = (float)ctx->viewport.height - 75;

	static std::string yaw_text;
	static std::string pitch_text;
	
	yaw_text = format_coord("Yaw", (float)camera->yaw);
	pitch_text = format_coord("Pitch", (float)camera->pitch);
	
	draw_text(bold_font, ctx, yaw_text, x, y, scale, GREEN_COLOR, ALPHA, align, DO_DROP_SHADOW);
	draw_text(bold_font, ctx, pitch_text, x, y - LINE_HEIGHT, scale, GREEN_COLOR, ALPHA, align, DO_DROP_SHADOW);
}


void display_sprint_status(const RenderingContext* ctx) {
	using namespace DebugOverlay;
	const Font* bold_font    = &ctx->assets.fonts[FONT_BOLD];
	const auto sprint_status = ctx->camera_data.camera.is_sprinting;

	const float scale = 0.5f;
	const TextAlign align = TextAlign::LEFT;

	const float x = 0;
	const float y = (float)ctx->viewport.height - 125;

	std::string sprint_text = "Sprinting: ";
	if (!sprint_status)
		sprint_text += "FALSE";
	else
		sprint_text += "TRUE";

	draw_text(bold_font, ctx, sprint_text, x, y, scale, GREEN_COLOR, ALPHA, align, DO_DROP_SHADOW);
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
