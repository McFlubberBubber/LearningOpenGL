#pragma once

#include <glad/glad.h>	//access to openGL functions
#include <glm/glm.hpp>
#include <string>


struct Shader {
	uint32_t id;

	const char* vertex_path;
	const char* fragment_path;
	const char* geometry_path;
	
	bool is_valid;
};

// We can optionally cache the uniform locations in the shader
/*
struct UniformCache {
	std::unordered_map<std::string, int32_t> uniform_locations;
	uint32_t id;
};
*/

// Shader creation and management
Shader create_shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr);
void destroy_shader(Shader* shader);

// Usage of shader
void use_shader(const Shader* shader);

// Uniform-setting functions
void set_bool(const Shader* shader, const char* name, bool value);
void set_int(const Shader* shader, const char* name, int value);
void set_float(const Shader* shader, const char* name, float value);


// Vector utilities
void set_vec2(const Shader* shader, const char* name, const glm::vec2 &value);
void set_vec2(const Shader* shader, const char* name, float x, float y);

void set_vec3(const Shader* shader, const char* name, const glm::vec3 &value);
void set_vec3(const Shader* shader, const char* name, float x, float y, float z);

void set_vec4(const Shader* shader, const char* name, const glm::vec4 &value);
void set_vec4(const Shader* shader, const char* name, float x, float y, float z, float w);


// Matrix utilities
void set_mat2(const Shader* shader, const char* name, const glm::mat2 &value);
void set_mat3(const Shader* shader, const char* name, const glm::mat3 &value);
void set_mat4(const Shader* shader, const char* name, const glm::mat4 &value);


// Setting uniform buffers to the shader
void set_uniform_buffer(const Shader* shader, const char* name, int binding_point);


// Helper functions
bool check_compile_errors(uint32_t shader, const char* type);


// OPTIONAL: Uniform location caching
/*
UniformCache create_uniform_cache(uint32_t id);
int32_t get_cached_uniform_location(UniformCache* cache, const char* name);
*/

