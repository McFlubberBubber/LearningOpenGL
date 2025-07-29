#pragma once
#ifndef RENDERING_H
#define RENDERING_H
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "UserInput.h"
#include "Skybox.h"

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern Shader screenShader;

// Initializing things for the game objects
void initBuffers(const unsigned int width, const unsigned int height);
void initShaders();
void initModels();
void initTextures();


// Will be used in render loop - utilises all the draw functions above
void renderScene(Camera& camera, const float ASPECT_RATIO);
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

#endif
