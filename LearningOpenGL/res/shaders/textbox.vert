#version 460 core
layout (location = 0) in vec2 pos;

uniform mat4 projection;

void main() {
	gl_Position = projection * vec4(pos, 0.0f, 1.0f);
	return;
}