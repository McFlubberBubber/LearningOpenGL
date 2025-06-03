#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>	//access to openGL functions

#include <iostream>
#include <fstream>		//file stream
#include <sstream>		//string stream
#include <string>

class Shader 
{
public:
	unsigned int programID;		//program ID

	//constructor that reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath) 
	{
		//1. retrieving the vertex + fragment source codes
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		//error handling
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			//a. opening files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;

			//b. reading the file's buffer contents
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			//c. closing the file handlers
			vShaderFile.close();
			fShaderFile.close();

			//d. converting the stream into a string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure error) 
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		//2. compiling shaders
		unsigned int vertex, fragment;
		
		//vertex shader compilation
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		m_checkCompileErrors(vertex, "VERTEX");

		//fragment shader compilation
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		m_checkCompileErrors(fragment, "FRAGMENT");

		//creating the shader program
		programID = glCreateProgram();
		glAttachShader(programID, vertex);
		glAttachShader(programID, fragment);
		glLinkProgram(programID);
		m_checkCompileErrors(programID, "PROGRAM");

		//deleting unused shaders since they are linked to program
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	//using + activating the shader
	void useProgram()
	{
		glUseProgram(programID);
	}

	//utilities for the uniform (just need to specify the name of the uniform variable, and the value they get set)
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
	}
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
	}
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(programID, name.c_str()), value);

	}

	//functions for setting vectors in uniforms
	void setVec2(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
	}
	//for setting specifically an x and y
	void setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
	}

	//setting vector 3
	void setVec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
	}
	//for setting specifically an x and y
	void setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
	}

	//setting vector 4
	void setVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
	}
	//for setting specifically an x and y
	void setVec4(const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
	}

	//functions for setting matrixes
	void setMat2(const std::string &name, const glm::mat2 &matrix) const
	{
		glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
	}
	void setMat3(const std::string &name, const glm::mat3 &matrix) const
	{
		glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
	}
	void setMat4(const std::string &name, const glm::mat4 &matrix) const
	{
		glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
	}

private:
	//function to check and log any shader compilation errors
	void m_checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[512];

		if (type != "PROGRAM")				//if the type specified is NOT of PROGRAM
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
			}
		}
		else								//else it would be a program error
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 512, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
			}

		}
	}
};

#endif
