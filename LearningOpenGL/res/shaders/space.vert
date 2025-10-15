#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 tex_coord;

out vec2 texture_coords;

layout (std140) uniform matrices {
	mat4 projection;
	mat4 view;
};

uniform mat4 model_matrix;

void main() {
	texture_coords = tex_coord;
	gl_Position = projection * view * model_matrix * vec4(pos, 1.0f);
	return;
}