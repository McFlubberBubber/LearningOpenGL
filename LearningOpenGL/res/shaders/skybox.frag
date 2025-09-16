#version 460 core

//INPUTS
in vec3 texture_output;

//OUTPUTS
out vec4 frag_color;

//UNIFORMS - automatically initialized to 0.
uniform samplerCube skybox;

void main () {
	frag_color = texture(skybox, texture_output);
}