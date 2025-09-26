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


// BIG REFACTOR THING
// Initialization functions
/*
bool init_camera(CameraData* camera_data);
bool init_textures(Assets* assets);
bool init_buffers(BufferData* buffers, GeometryData* geometry, u32 texture_color_buffer);
bool init_shaders(Assets* assets);
bool init_models(Assets* assets);
bool init_fonts(Assets* assets);
*/

bool init_rendering_system(RenderingContext* context);

// Cleanup functions
/*
void cleanup_buffers(BufferData* buffers);
void cleanup_shaders(Assets* assets);
void cleanup_models(Assets* assets);
void cleanup_textures(Assets* assets);
void cleanup_fonts(Assets* assets);
*/

bool cleanup_rendering_system(RenderingContext* context);

// Utility functions
void apply_matrices(const Shader* shader);
void process_lighting(const Shader* shader, const RenderingContext* ctx);
void bind_textures(const Shader* shader, u32 diffuse, u32 specular, u32 emission);
void set_texture_uniforms(const Shader* shader, bool do_emission);
void update_camera_projection(RenderingContext* ctx);
void resize_framebuffer(const RenderingContext* context, u32 width, u32 height);

// @TODO: This is required by the input thing, will probably be moved
// void apply_render_mode_to_screen_shader(RenderMode render_mode);
void apply_render_mode_to_screen_shader(const RenderingContext* context);


