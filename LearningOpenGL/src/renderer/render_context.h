#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/render_data.h"
#include "renderer/camera.h"
#include "renderer/post_processing.h"
#include "core/program_state.h"
#include "core/vars.h"

// This struct being named RenderingContext is a really bad name as we are just
// stuffing things in here that does not relate to it. We would probably have
// another struct named like "Application", that would have the RenderingContext
// in there, and then we can include our input stuff, interfaces, etc.

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

	HotloadedVariables vars;
};
