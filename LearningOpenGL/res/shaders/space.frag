#version 460 core

struct Material {
	sampler2D diffuse1;
};

in vec2 texture_coords;

out vec4 frag_color;

uniform Material material;

void main() {
	frag_color = texture(material.diffuse1, texture_coords);

	return;
}