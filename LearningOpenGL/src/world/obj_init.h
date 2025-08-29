#pragma once

//Forward declarations
struct WorldObjectData;
struct LightingData;

// Initializing functions for all the objects that will get rendered on the scene (world positions)
void init_world_objects(WorldObjectData* world);
void init_lighting(LightingData* lights);