#version 460 core

//INPUTS
in vec3 textureOutput;

//OUTPUTS
out vec4 FragColor;

//UNIFORMS
uniform samplerCube u_skybox;

void main () {
	FragColor = texture(u_skybox, textureOutput);
}