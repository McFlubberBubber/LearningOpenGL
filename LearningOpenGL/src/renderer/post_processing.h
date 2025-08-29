#pragma once


enum class RenderMode {
	NORMAL,
	INVERT,
	GRAYSCALE,
	SHARPEN,
	DARK_SHARPEN,
	COUNT		// Extra enum type
};

struct PostProcessingData {
	RenderMode mode { RenderMode::NORMAL };
};

void cycle_render_mode(PostProcessingData* data, bool forward = true);
