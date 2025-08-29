#include "shader.h"

#include <iostream>
#include <fstream>		//file stream
#include <sstream>		//string stream

Shader create_shader(const char* vertex_path, const char* fragment_path, const char* geometry_path) {
	Shader shader = {};
	shader.vertex_path   = vertex_path;
	shader.fragment_path = fragment_path;
	shader.geometry_path = geometry_path;
	shader.is_valid = false;

	// Reading the shader source code.
	std::string vertex_code, fragment_code, geometry_code;
	std::ifstream v_shader_file, f_shader_file, g_shader_file;

	//error handling
	v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	g_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		// Opening the files
		v_shader_file.open(vertex_path);
		f_shader_file.open(fragment_path);
		std::stringstream v_shader_stream, f_shader_stream;

		// Reading the buffer contents
		v_shader_stream << v_shader_file.rdbuf();
		f_shader_stream << f_shader_file.rdbuf();

		// Closing the file handlers
		v_shader_file.close();
		f_shader_file.close();

		// Converting stream into a string
		vertex_code   = v_shader_stream.str();
		fragment_code = f_shader_stream.str();

		// OPTIONAL: If there is a geometry shader, then do the same process.
		if (geometry_path != nullptr) {
			g_shader_file.open(geometry_path);
			std::stringstream g_shader_stream;

			g_shader_stream << g_shader_file.rdbuf();
			g_shader_file.close();

			geometry_code = g_shader_stream.str();
			std::cout << "Geometry shader loaded at path: " << geometry_path << "\n";
		}

		// Logging file paths
		std::cout << "Vertex shader loaded at path: " << vertex_path << "\n";
		std::cout << "Fragment shader loaded at path: " << fragment_path << "\n";
	}
	catch (std::ifstream::failure error) {
		std::cout << "ERROR: Shader file not successfully read (possible invalid path?)" << std::endl;
		return shader;
	}

	// Compiling shaders
	const char* v_shader_code = vertex_code.c_str();
	const char* f_shader_code = fragment_code.c_str();

	uint32_t vertex, fragment, geometry;

	// VERTEX
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_shader_code, NULL);
	glCompileShader(vertex);
	if (!check_compile_errors(vertex, "VERTEX")) {
		glDeleteShader(vertex);
		return shader;
	}

	// FRAGMENT
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_shader_code, NULL);
	glCompileShader(fragment);
	if (!check_compile_errors(fragment, "FRAGMENT")) {
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return shader;
	}

	// GEOMETRY (if present)
	if (geometry_path != nullptr) {
		const char* g_shader_code = geometry_code.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &g_shader_code, NULL);
		glCompileShader(geometry);
		if (!check_compile_errors(geometry, "GEOMETRY")) {
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			glDeleteShader(geometry);
			return shader;
		}
	}


	// Creating the shader program
	shader.id = glCreateProgram();
	glAttachShader(shader.id, vertex);
	glAttachShader(shader.id, fragment);
	if (geometry_path != nullptr)
		glAttachShader(shader.id, geometry);
	glLinkProgram(shader.id);

	// Checking for compile errors
	if (!check_compile_errors(shader.id, "PROGRAM")) {
		glDeleteProgram(shader.id);
		shader.id = 0;
	} else {
		shader.is_valid = true;
	}


	// Cleanup
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometry_path != NULL)
		glDeleteShader(geometry);

	return shader;
}

void use_shader(const Shader* shader) {
	if (shader && shader->is_valid)
		glUseProgram(shader->id);
}


// Uniform-setting values
void set_bool(const Shader* shader, const char* name, bool value) {
	if (!shader || !shader->is_valid) {
		std::cout << "ERROR: can't set_bool, shader is invalid!" << std::endl;
		return;
	}
	glUniform1i(glGetUniformLocation(shader->id, name), (int)value);
}

void set_int(const Shader* shader, const char* name, int value) {
	if (!shader || !shader->is_valid) {
		std::cout << "ERROR: can't set_int, shader is invalid!" << std::endl;
		return;
	}
	glUniform1i(glGetUniformLocation(shader->id, name), value);
}

void set_float(const Shader* shader, const char* name, float value) {
	if (!shader || !shader->is_valid) {
		std::cout << "ERROR: can't set_float, shader is invalid!" << std::endl;
		return;
	}
	glUniform1f(glGetUniformLocation(shader->id, name), value);
}


// Vector stuff
void set_vec2(const Shader* shader, const char* name, const glm::vec2 &value) {
	if (!shader || !shader->is_valid) {
		std::cout << "ERROR: can't set_vec2, shader is invalid!" << std::endl;
		return;
	}
	glUniform2fv(glGetUniformLocation(shader->id, name), 1, &value[0]);
}

void set_vec2(const Shader* shader, const char* name, float x, float y) {
	if (!shader || !shader->is_valid) {
		std::cout << "ERROR: can't set_vec2, shader is invalid!" << std::endl;
		return;
	}
	glUniform2f(glGetUniformLocation(shader->id, name), x, y);
}


void set_vec3(const Shader* shader, const char* name, const glm::vec3 &value) {
	if (!shader || !shader->is_valid) {
		std::cout << "ERROR: can't set_vec3, shader is invalid!" << std::endl;
		return;
	}
	glUniform3fv(glGetUniformLocation(shader->id, name), 1, &value[0]);
}

void set_vec3(const Shader* shader, const char* name, float x, float y, float z) {
	if (!shader || !shader->is_valid) {
		std::cout << "ERROR: can't set_vec3, shader is invalid!" << std::endl;
		return;
	}
	glUniform3f(glGetUniformLocation(shader->id, name), x, y, z);
}


void set_vec4(const Shader* shader, const char* name, const glm::vec4 &value) {
	if (!shader || !shader->is_valid) {
		std::cout << "ERROR: can't set_vec4, shader is invalid!" << std::endl;
		return;
	}
	glUniform4fv(glGetUniformLocation(shader->id, name), 1, &value[0]);
}

void set_vec4(const Shader* shader, const char* name, float x, float y, float z, float w) {
	if (!shader || !shader->is_valid) {
		std::cout << "ERROR: can't set_vec4, shader is invalid!" << std::endl;
		return;
	}
	glUniform4f(glGetUniformLocation(shader->id, name), x, y, z, w);
}


// Matrix stuff
void set_mat2(const Shader* shader, const char* name, const glm::mat2 &value) {
	glUniformMatrix2fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, &value[0][0]);
}

void set_mat3(const Shader* shader, const char* name, const glm::mat3 &value) {
	glUniformMatrix3fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, &value[0][0]);
}

void set_mat4(const Shader* shader, const char* name, const glm::mat4 &value) {
	glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, &value[0][0]);
}


// For setting uniform buffers
void set_uniform_buffer(const Shader* shader, const char* name, int binding_point) {
	uint32_t uniform_block_index = glGetUniformBlockIndex(shader->id, name);
	if (uniform_block_index == GL_INVALID_INDEX) {
		std::cout << "WARNING: Uniform block = " << name << ", not found in shader\n";
		return;
	}
	glUniformBlockBinding(shader->id, uniform_block_index, binding_point);
}

// Helper functions
bool check_compile_errors(uint32_t shader, const char* type) {
	int success;
	char info_log[512];

	if (strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, info_log);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log << std::endl;
            return false;
        }

    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, info_log);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log << std::endl;
            return false;
        }
    }
    return true;
}
