#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/render_data.h"
#include "renderer/camera.h"
#include "renderer/post_processing.h"
#include "core/program_state.h"
 
struct RenderingContext {
	Assets assets;

	CameraData camera_data;
	BufferData buffers;
	GeometryData geometry;
	WorldObjectData world;
	LightingData lighting;

	ViewportState viewport;
	MessageQueue message_queue;
	PostProcessingData post_processing;

	ApplicationState* app_state;

	// Temporary flag for rendering debug overlay
	bool debug_mode = false;
};
