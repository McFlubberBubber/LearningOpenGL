#version 330 core

// INPUTS
in vec3 normal;
in vec3 position;

// OUTPUTS
out vec4 FragColor;

// UNIFORMS
uniform vec3 u_viewPosition;
uniform samplerCube u_skybox;

void main () {
	// Using glass refractive index
	float ratio = 1.00f / 1.52f;
	
	vec3 i = normalize(position - u_viewPosition);
	vec3 r = refract(i, normalize(normal), ratio);
	
	FragColor = vec4(texture(u_skybox, r).rgb, 1.0f);
}