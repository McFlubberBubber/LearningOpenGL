#version 460 core

// Inputs
in vec2 texture_coords;

// Outputs
out vec4 color;

// Uniforms
uniform sampler2D text;
uniform vec3 text_color;
uniform float text_alpha;

void main () {
	vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(text, texture_coords).r);
	color = vec4(text_color, text_alpha) * sampled;
}