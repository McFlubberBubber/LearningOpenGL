#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;


// Trying out interface blocks for outputs
out VS_OUT {
	vec3 normal;
	vec3 position;
} vs_out;


// Instead of just creating new uniforms,  we can use a uniform buffer object for
// reusable uniforms that are applicable to most shaders (in this case, the projection
// and view matrixes can be used in a uniform block).

layout (std140) uniform matrices {
	mat4 projection;
	mat4 view;
};

uniform mat4 model_matrix; // Model matrixes vary between most shaders so this will need to be set manually


void main () {
	vs_out.normal = mat3(transpose(inverse(model_matrix))) * norm;
	vs_out.position = vec3(model_matrix * vec4(pos, 1.0f));

	gl_Position = projection * view * vec4(vs_out.position, 1.0f);
}
