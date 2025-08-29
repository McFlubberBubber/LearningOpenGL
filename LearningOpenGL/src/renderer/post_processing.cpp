#include "renderer/post_processing.h"

#include "renderer/render_context.h"
#include "core/types.h"

void cycle_render_mode(PostProcessingData* data, bool forward) {
	s32 current = static_cast<s32>(data->mode);
	s32 total   = static_cast<s32>(RenderMode::COUNT);

	if (forward)
		data->mode = static_cast<RenderMode>((current + 1) % total);
	else
		data->mode = static_cast<RenderMode>((current - 1 + total) % total);
}



