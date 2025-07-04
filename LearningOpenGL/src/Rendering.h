#pragma once
#ifndef RENDERING_H
#define RENDERING_H

#include "Shader.h"
#include "Model.h"
#include "Camera.h"

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Initializing things for the game objects
void initBuffers();
void initShaders();
void initModels();
void initTextures();

// Drawing the cubes
void drawWoodenContainers();
void drawEmissionContainer();
void drawContainers();

//Drawing the models
void drawBackpack();
void drawBlahaj();
void drawModels();

//Drawing light sources
void drawPointLights();
void drawDirectionalLight();
void drawLights();

//Room rendering
void drawGrass();
void drawFloor();
void drawWalls();
void drawRoom();

// Will be used in render loop - utilises all the draw functions above
void renderScene(Camera& camera, const float ASPECT_RATIO);
void cleanupScene();

// Utility functions for drawing
void applyMatrixes(Shader& shader);
unsigned int loadTexture(const char* path);
void processLighting(Shader& shader);
glm::vec3 calculateSkyColor(float currentTime);

#endif