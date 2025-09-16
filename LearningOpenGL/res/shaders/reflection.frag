#version 460 core

/*
in vec3 normal;
in vec3 position;
*/


// Using interface blocks to take in inputs
in VS_OUT {
	vec3 normal;
	vec3 position;
} fs_in;


// OUTPUTS
out vec4 frag_color;


// UNIFORMS
uniform vec3 view_position;
uniform samplerCube skybox;


void main () {
	vec3 i = normalize(fs_in.position - view_position);
	vec3 r = reflect(i, normalize(fs_in.normal));

	frag_color = vec4(texture(skybox, r).rgb, 1.0f);
}