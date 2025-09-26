#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

out VS_OUT {
	vec3 normal;
} vs_out;


layout (std140) uniform matrices {
	mat4 projection;
	mat4 view;
};

uniform mat4 model_matrix;

void main() {
	mat3 normal_matrix = mat3(transpose(inverse(view * model_matrix)));
    vs_out.normal = vec3(vec4(normal_matrix * norm, 0.0f));
    gl_Position = view * model_matrix * vec4(pos, 1.0f); 

	return;
}
