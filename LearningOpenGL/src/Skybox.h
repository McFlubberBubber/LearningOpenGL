#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include "Camera.h"

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern Shader skybox_shader;
// extern Shader reflection_shader;

unsigned int load_cubemap(std::vector<std::string> faces);
void init_skybox();
void init_reflection_cube();
void init_refraction_cube();

void draw_skybox(const glm::mat4& projection_matrix, const Camera& camera);
void draw_reflection_cube(const glm::mat4& projection_matrix, const glm::vec3& view_position, const glm::mat4& view_matrix);
void draw_refraction_cube(const glm::mat4& projection_matrix, const glm::vec3& view_position, const glm::mat4& view_matrix);

#endif
