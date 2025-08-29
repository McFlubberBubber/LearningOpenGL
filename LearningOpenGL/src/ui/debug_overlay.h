#pragma once

#include <string>
//#include "renderer/render_context.h"
//#include "renderer/camera.h"
//#include "renderer/post_processing.h"

// Forward declarations
enum class CameraMode;
enum class RenderMode;

struct Assets;
struct ViewportState;
struct RenderingContext;


// Main function that will keep rendering all UI elements
void render_debug_overlay(RenderingContext* context, float dt);

// Pop up texts
void display_camera_mode_status(Assets* assets, CameraMode mode);
void display_render_mode_status(Assets* assets, const ViewportState* viewport, RenderMode mode);

// Consistent UI
void display_fps(const RenderingContext* context, float dt);
void display_coords(const RenderingContext* context);


// Helper functions
std::string render_mode_to_string(RenderMode render_mode);
std::string format_coord(const std::string& label, float value);
