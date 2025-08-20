#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>	//access to openGL functions
#include <glm/glm.hpp>
#include <string>

class Shader {
public: 
	unsigned int programID = 0;
	Shader() = default;

	// constructor + using the program based on ID
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometry_path);
	void useProgram();

	//utilities for setting uniforms
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;


	//vector uniforms
	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setVec2(const std::string &name, float x, float y) const;

	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;

	void setVec4(const std::string &name, const glm::vec4 &value) const;
	void setVec4(const std::string &name, float x, float y, float z, float w) const;


	//matrix uniforms
	void setMat2(const std::string &name, const glm::mat2 &matrix) const;
	void setMat3(const std::string &name, const glm::mat3 &matrix) const;
	void setMat4(const std::string &name, const glm::mat4 &matrix) const;


	// @INCOMPLETE: Uniform buffer binding
	void set_uniform_buffer(const std::string &uniform_block_name, const int binding_point) const;
	

private:
	//logging any compilation errors of any specific shader type
	void m_checkCompileErrors(unsigned int shader, std::string type);
};

#endif
