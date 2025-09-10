#pragma once

#include "renderer/shader.h"
#include "renderer/model.h"
#include "renderer/text_rendering.h"
#include "core/types.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


// Forward declarations
struct Model;
struct Shader;
struct Font;


// These enums will list out all the render-able things such as shaders, models,
// textures, fonts, etc.
enum ShaderType {
	SHADER_CONTAINER = 0,
	SHADER_EMISSION,
	SHADER_LIGHT_CUBE,

	SHADER_FLOOR,
	SHADER_WALL,
	SHADER_WINDOW,
	SHADER_GRASS,

	SHADER_BACKPACK,
	SHADER_BLAHAJ,
	SHADER_HOUSE,
	SHADER_EXPLODE_MODEL,

	SHADER_SCREEN,
	SHADER_FONT,
	SHADER_LINE,

	SHADER_SKYBOX,
	SHADER_REFLECTION_CUBE,
	SHADER_REFRACTIVE_CUBE,

	SHADER_COUNT
};

enum ModelType {
	MODEL_BACKPACK = 0,
	MODEL_BLAHAJ,
	MODEL_HOUSE,

	MODEL_COUNT
};

// @Hardcode: the way we have these textures listed is VERY important since the
// init_textures() in rendering.h / cpp initializes these textures in the same
// order. This can be changed by using a hash map, but this approach is currently
// easier and less overhead (for now). If we ever get into loading a bunch more
// textures, then we will probably swap.
enum TextureType {
	TEXTURE_DIFFUSE = 0,
	TEXTURE_SPECULAR,
	TEXTURE_EMISSION,

	TEXTURE_FLOOR,
	TEXTURE_WALL,
	TEXTURE_WINDOW,
	TEXTURE_GRASS,
	TEXTURE_GRASSLAND,

	TEXTURE_SKYBOX,
	TEXTURE_COLOR_BUFFER,

	TEXTURE_COUNT
};

enum FontType {
	FONT_REGULAR = 0,
	FONT_BOLD,
	FONT_ITALIC,
//	FONT_FPS,

	FONT_TITLE,
	FONT_BODY,
	FONT_SMALL,

	FONT_COUNT
};

struct Assets {
	// These can be changes into dynamic arrays with std::vector.
	// std::vector<Shader> shaders;
	// std::unordered_map<std::string, uint32_t> shader_indices;
	
	Shader shaders[SHADER_COUNT];
	Model models[MODEL_COUNT];
	u32 textures[TEXTURE_COUNT];
	Font fonts[FONT_COUNT];
};


struct BufferData {
	u32 cube_VAO, cube_VBO;
	u32 wall_VAO, wall_VBO;
	u32 line_VAO, line_VBO;

	u32 skybox_VAO, skybox_VBO;
	u32 special_cube_VAO, special_cube_VBO;		// For reflection + refractive cubes

//	u32 EBO;									// Not used currently
	u32 quad_VAO, quad_VBO;
	u32 FBO, RBO;								// Frame + render buffers
	u32 UBO_matrices;							// Uniform buffer (view matrices)
};


struct WorldObjectData {
	std::vector<glm::vec3> cube_positions;
	std::vector<glm::vec3> wall_positions;
	std::vector<glm::vec3> foliage_positions;
	std::vector<glm::vec3> window_positions;
	std::vector<glm::vec3> blahaj_positions;
	std::vector<float> wall_rotations;


	void reserve_space(u32 cubes, u32 walls, u32 foliage, u32 windows, u32 blahajs) {
		cube_positions.reserve(cubes);
		
		wall_positions.reserve(walls);
		wall_rotations.reserve(walls);
		
		foliage_positions.reserve(foliage);
		window_positions.reserve(windows);
		blahaj_positions.reserve(blahajs);
	}


	void clear_all() {
		cube_positions.clear();
		wall_positions.clear();
		foliage_positions.clear();
		blahaj_positions.clear();
		wall_rotations.clear();
	}
};


struct GeometryData {
	// Positions, normals and textures
	static constexpr float cube_vertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,	   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,	   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,	   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		// Front face
		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		// Left face
		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f, 0.0f,   1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f, 0.0f,   1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f, 0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f, 0.0f,   1.0f, 0.0f,

		// Right face
		0.5f,  0.5f,  0.5f,   1.0f,  0.0f, 0.0f,   1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,   1.0f,  0.0f, 0.0f,   1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,   1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,   1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,   1.0f,  0.0f, 0.0f,   0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,   1.0f,  0.0f, 0.0f,   1.0f, 0.0f,

		// Bottom face
		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,

		// Top face
		-0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,    0.0f,  1.0f, 0.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,    0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,    0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f
	};

	// Positions, normals, textures
	static constexpr float wall_vertices[] = {
		-0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

		0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		-0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		-0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	};

	// Positions, textures
	static constexpr float quad_vertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	// Normalised coords on the screen
	static constexpr float points[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
		0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
	};

	// This is used for the reflections + refraction cubes (position and normal only)
	static constexpr float cube_vertices_2[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	// Skybox vertices (just needs the positions)
	static constexpr float skybox_vertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};


	// Vertex counts just incase
	static constexpr u32 cube_vertex_count		= sizeof(cube_vertices) / sizeof(float);
	static constexpr u32 cube_vertex_2_count	= sizeof(cube_vertices_2) / sizeof(float);
	static constexpr u32 wall_vertex_count		= sizeof(wall_vertices) / sizeof(float);
	static constexpr u32 quad_vertex_count		= sizeof(quad_vertices) / sizeof(float);
	static constexpr u32 skybox_vertex_count	= sizeof(skybox_vertices) / sizeof(float);

};


struct LightingData {
	// Directional Lighting properties
	glm::vec3 directional_light_dir;
	glm::vec3 directional_ambient;
	glm::vec3 directional_diffuse;
	glm::vec3 directional_specular;

	// Point Lighting
	std::vector<glm::vec3> point_light_positions;
	std::vector<glm::vec3> point_light_colors;

	// Sky Lighting
	glm::vec3 dark_sky_color;
	glm::vec3 grey_sky_color;
	glm::vec3 current_sky_color;


	inline void reserve_point_lights(u32 count) {
		point_light_positions.reserve(count);
		point_light_colors.reserve(count);
	}

	inline u32 get_point_light_count() const {
		return (u32)point_light_positions.size();
	}

	inline void apply_sky_color(float current_time) {
		const float transition_speed = 0.3f;
		const float t = 0.5f * (1.0f + sin(transition_speed * current_time));
		current_sky_color = glm::mix(dark_sky_color, grey_sky_color, t);
	}
};
