#version 460 core

layout (location = 0) in vec4 vertex;

// Outputs
out vec2 texture_coords;

// Uniform
uniform mat4 projection;

void main () {
	gl_Position = projection * vec4(vertex.xy, 0.0f, 1.0f);
	texture_coords = vertex.zw;
}