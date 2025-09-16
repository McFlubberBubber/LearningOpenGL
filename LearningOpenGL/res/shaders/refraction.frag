#version 460 core

/*
in vec3 normal;
in vec3 position;
*/

in VS_OUT {
   in vec3 normal;
   in vec3 position;	
} fs_input;


// OUTPUTS
out vec4 frag_color;

// UNIFORMS
uniform vec3 view_position;
uniform samplerCube skybox;

void main () {
	// Using glass refractive index
	float ratio = 1.00f / 1.52f;
	
	vec3 i = normalize(fs_input.position - view_position);
	vec3 r = refract(i, normalize(fs_input.normal), ratio);
	
	frag_color = vec4(texture(skybox, r).rgb, 1.0f);
}