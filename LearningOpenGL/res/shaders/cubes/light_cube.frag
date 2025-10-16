#version 460 core
//OUTPUTS
out vec4 frag_color;

//UNIFORMS
uniform vec3 light_color;
uniform vec3 sky_color;
uniform float fog_distance;

//FUNCTION PROTOTYPES
float linearize_depth(float depth);

//global variables for fog
float near = 0.1f;
float far = 100.0f;

void main()
{
	//visualizing the depth buffer with foggyness
	float fog_density = fog_distance;
	float depth = linearize_depth(gl_FragCoord.z) / far;
	//frag_color = vec4(vec3(depth), 1.0);
	float depth_vec = exp(-pow(depth * fog_density, 2.0));

	//using different fog colors for testing
	vec3 fog_color = sky_color;

	//mixing the result with the fog
	vec3 mixed_result = mix(fog_color, light_color, depth_vec);
	frag_color = vec4(mixed_result, 1.0f);
}

float linearize_depth(float depth){
    float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near) );
}
