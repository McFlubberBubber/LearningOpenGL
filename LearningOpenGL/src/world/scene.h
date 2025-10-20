#pragma once

// Forward declarations
struct RenderingContext;

void generate_blahaj_matrices(RenderingContext* ctx);
void render_scene(RenderingContext* context, float dt);
void cleanup_main_scene();
