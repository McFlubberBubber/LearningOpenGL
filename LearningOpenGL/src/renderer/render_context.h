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
	ApplicationState app;	// @TODO: Does this structure really belong here?

	PostProcessingData post_processing;


	// Temporary flag for rendering debug overlay
	bool debug_mode = false;
};
