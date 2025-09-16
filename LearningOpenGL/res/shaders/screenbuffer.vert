#version 460 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coords;

out vec2 texture_output;

void main () {
	gl_Position = vec4(pos.x, pos.y, 0.0f, 1.0f);
	texture_output = tex_coords;
}