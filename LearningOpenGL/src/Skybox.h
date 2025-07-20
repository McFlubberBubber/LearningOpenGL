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

void init_skybox();
unsigned int load_cubemap(std::vector<std::string> faces);

void draw_skybox(const glm::mat4& projection_matrix, const Camera& camera);

#endif
