#version 460 core

// INPUTS
in vec3 fColor;

// OUTPUTS
out vec4 FragColor;


void main() {
	// Outputting a green color.
	FragColor = vec4(fColor, 1.0f);
}
