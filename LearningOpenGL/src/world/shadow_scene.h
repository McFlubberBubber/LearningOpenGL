#pragma once

#include "core/types.h"

struct RenderingContext;

extern u32 SHADOW_WIDTH;
extern u32 SHADOW_HEIGHT;

void init_shadow_scene(RenderingContext* ctx);
void render_shadow_scene(RenderingContext* ctx, float dt);

