#version 460 core

out vec4 frag_color;

uniform vec3 color;
uniform float alpha;

void main() {
	frag_color = vec4(color, alpha);
	return;
}