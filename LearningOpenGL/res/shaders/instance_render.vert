#version 460 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 offset;

out vec3 color;

uniform vec2 offsets[100];

void main() {
	vec2 position = pos * (gl_InstanceID / 100.0);
	gl_Position = vec4(position + offset, 0.0, 1.0);
	color = col;

	return;
}