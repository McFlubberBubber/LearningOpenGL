#include "obj_init.h"

#include "renderer/render_data.h"

void init_world_objects (WorldObjectData* world) {
	// Initializing cube positions
	world->cube_positions = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// Initializing walls (including rotations)
	world->wall_positions = {
		glm::vec3(-15.0f, -4.0f, 5.0f),  // left wall
		glm::vec3(-5.0f, -4.0f, 5.0f),	// right wall
		glm::vec3(-10.0f, -4.0f, 0.0f),	// front wall
		glm::vec3(-10.0f, -4.0f, 10.0f)	// back wall
	};

	world->wall_rotations = { 0.0f, 0.0f, 90.0f, 90.0f };

	
	// Initializing foliage
    world->foliage_positions = {
        glm::vec3(0.0f, -4.5f, 2.0f),
        glm::vec3(1.0f, -4.5f, 0.0f),
        glm::vec3(4.0f, -4.5f, 5.0f),
        glm::vec3(-2.0f, -4.5f, -1.0f),
        glm::vec3(-5.0f, -4.5f, -1.0f),
        glm::vec3(5.0f, -4.5f, 3.0f),
        glm::vec3(3.5f, -4.5f, -6.0f),
        glm::vec3(2.0f, -4.5f, -3.5f),
        glm::vec3(-2.0f, -4.5f, -7.0f),
        glm::vec3(7.0f, -4.5f, 2.0f)
    };


	// Initializing windows    
    world->window_positions = {
        glm::vec3(5.0f, -4.5f, 5.0f),
        glm::vec3(3.0f, -4.5f, 2.0f),
        glm::vec3(5.0f, -4.5f, 0.0f)
    };


	// Initializing blahajs
	world->blahaj_positions = {
        glm::vec3(5.0f, 5.0f, -5.0f),
        glm::vec3(7.0f, 2.0f, 7.0f),
        glm::vec3(-6.0f, -1.0f, -5.0f),
        glm::vec3(4.0f, -3.0f, -1.0f),
        glm::vec3(5.0f, 0.0f, 5.0f)
    };
}


void init_lighting(LightingData *lights) {
	// Directional Lighting
	lights->directional_light_dir = glm::vec3(-5.0f, 10.0f, -5.0f);
	lights->directional_ambient   = glm::vec3(0.3f, 0.35f, 0.4f);
	lights->directional_diffuse   = glm::vec3(1.0f, 0.95f, 0.8f);
	lights->directional_specular  = glm::vec3(1.0f, 1.0f, 0.9f);


	// Point Lighting
	lights->point_light_positions = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	lights->point_light_colors = {
		glm::vec3(0.75f, 0.75f, 0.75f),
		glm::vec3(0.75f, 0.0f, 0.60f),
		glm::vec3(0.0f, 0.0f, 0.8f),
		glm::vec3(0.75f, 0.05f, 0.05f)
	};

	// Initializing the sky colors
	lights->dark_sky_color = glm::vec3(0.001f, 0.001f, 0.001f);
	lights->grey_sky_color = glm::vec3(0.5f, 0.5f, 0.5f);
	// lights->current_sky_color = lights->grey_sky_color; // Currently overriden in the main scene anyways.

	// Sunlight stuff for space scene
	lights->sunlight_position = glm::vec3(0.0f, 0.0f, -75.0f);
	lights->sunlight_color	  = glm::vec3(1.0f);
}
