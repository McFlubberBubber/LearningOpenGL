#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/types.h"

// Forward declarations
struct Shader;
struct RenderingContext;
struct CameraData;
struct ViewportState;

// Management of the rendering system.
bool init_rendering_system(RenderingContext* context);
bool cleanup_rendering_system(RenderingContext* context);

// Utility functions.
void apply_matrices(const Shader* shader);
void process_lighting(const Shader* shader, const RenderingContext* ctx);
void bind_textures(const Shader* shader, u32 diffuse, u32 specular, u32 emission);
void set_texture_uniforms(const Shader* shader, bool do_emission);
void update_camera_projection(RenderingContext* ctx);
void resize_framebuffer(RenderingContext* context, u32 width, u32 height);
void apply_render_mode_to_screen_shader(const RenderingContext* context);
void draw_screen_texture(RenderingContext* ctx);
