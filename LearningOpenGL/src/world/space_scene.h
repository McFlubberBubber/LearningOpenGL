#pragma once

// Forward declarations
struct RenderingContext;

void generate_rock_matrices(RenderingContext* ctx);
void render_space_scene(RenderingContext* ctx, float dt);
void cleanup_space_scene();

void validate_rock_instancing(RenderingContext* ctx);
