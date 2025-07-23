#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;
out vec3 position;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;

void main () {
	normal = mat3(transpose(inverse(u_modelMatrix))) * aNormal;
	position = vec3(u_modelMatrix * vec4(aPos, 1.0f));

	gl_Position = u_projectionMatrix * u_viewMatrix * vec4(position, 1.0f);
}
