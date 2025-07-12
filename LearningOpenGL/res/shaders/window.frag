#version 330 core

//INPUTS
in vec3 normalOutput;
in vec3 fragPosOutput;
in vec2 textureOutput;

//OUTPUTS
out vec4 FragColor;

//UNIFORMS
uniform sampler2D u_texture1;

void main() {
	FragColor = texture(u_texture1, textureOutput);
}