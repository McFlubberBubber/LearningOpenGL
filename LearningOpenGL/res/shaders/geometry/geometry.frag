#version 460 core

// INPUTS
in vec3 f_color;

// OUTPUTS
out vec4 frag_color;

void main() {
	// Outputting a green color.
	frag_color = vec4(f_color, 1.0f);
}
