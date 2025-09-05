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


/*
extern Shader screenShader;

// Initializing things for the game objects
void initBuffers();
void initShaders();
void initModels();
void initTextures();
void init_fonts();

// Will be used in render loop - utilises all the draw functions above
void render_UI(Camera &camera);
void display_fps();
void display_world_coords(Camera &camera);
void render_scene(Camera& camera);
void cleanupScene();

// Utility functions 
void applyMatrixes(Shader& shader);
unsigned int loadTexture(const char* path);
void processLighting(Shader& shader);
glm::vec3 calculateSkyColor(float currentTime);
void resize_framebuffer(int width, int height);
void apply_render_mode_to_screen_shader(RenderMode render_mode);
void set_texture_uniforms(Shader& shader, bool do_emission_setting);
void bind_textures(Shader& shader, unsigned int diffuse, unsigned int specular, unsigned int emission); 
void delete_vertex_data(unsigned int& VAO, unsigned int& VBO);
*/




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

// May move these functions to a scene.h instead
/*
// Functions that will draw objects
void draw_wooden_containers(const RenderingContext* context);
void draw_emission_container(const RenderingContext* context);
void draw_walls(const RenderingContext* context);
void draw_foliage(const RenderingContext* context);
void draw_windows(const RenderingContext* context);

void draw_backpack(const RenderingContext* context);
void draw_blahaj(const RenderingContext* context);
void draw_house(const RenderingContext* context);
void draw_explode_model(const RenderingContext* context);

void draw_point_lights(const RenderingContext* context);
void draw_directional_light(const RenderingContext* context);


// Higher-level draw / render calls
void draw_containers(const RenderingContext* context);
void draw_models(const RenderingContext* context);
void draw_lights(const RenderingContext* context);
void render_scene(const RenderingContext* context);
*/

// Utility functions
void apply_matrices(const Shader* shader);
void process_lighting(const Shader* shader, const RenderingContext* context);
void bind_textures(const Shader* shader, u32 diffuse, u32 specular, u32 emission);
void set_texture_uniforms(const Shader* shader, bool do_emission);
void update_camera_projection(RenderingContext* ctx);
void resize_framebuffer(const RenderingContext* context, u32 width, u32 height);

// @TODO: This is required by the input thing, will probably be moved
// void apply_render_mode_to_screen_shader(RenderMode render_mode);
void apply_render_mode_to_screen_shader(const RenderingContext* context);


