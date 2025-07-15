#version 330 core

//INPUTS
in vec2 textureOutput;

//OUTPUTS
out vec4 FragColor;

//UNIFORMS
uniform sampler2D u_screenTexture;

void main() {
	FragColor = texture(u_screenTexture, textureOutput);
}