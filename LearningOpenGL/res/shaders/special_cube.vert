#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;


// Trying out interface blocks for outputs
out VS_OUT {
	vec3 normal;
	vec3 position;
} vs_output;


// Instead of just creating new uniforms,  we can use a uniform buffer object for
// reusable uniforms that are applicable to most shaders (in this case, the projection
// and view matrixes can be used in a uniform block).

layout (std140) uniform u_matrices {
	mat4 u_projectionMatrix;
	mat4 u_viewMatrix;
};

uniform mat4 u_modelMatrix; // Model matrixes vary between most shaders so this will need to be set manually


void main () {
	vs_output.normal = mat3(transpose(inverse(u_modelMatrix))) * aNormal;
	vs_output.position = vec3(u_modelMatrix * vec4(aPos, 1.0f));

	gl_Position = u_projectionMatrix * u_viewMatrix * vec4(vs_output.position, 1.0f);
}
