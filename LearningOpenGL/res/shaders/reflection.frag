#version 460 core

/*
in vec3 normal;
in vec3 position;
*/


// Using interface blocks to take in inputs
in VS_OUT {
	vec3 normal;
	vec3 position;
} fs_input;


// OUTPUTS
out vec4 FragColor;


// UNIFORMS
uniform vec3 u_viewPosition;
uniform samplerCube u_skybox;


void main () {
	vec3 i = normalize(fs_input.position - u_viewPosition);
	vec3 r = reflect(i, normalize(fs_input.normal));

	FragColor = vec4(texture(u_skybox, r).rgb, 1.0f);
}