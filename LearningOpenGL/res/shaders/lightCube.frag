#version 460 core
//OUTPUTS
out vec4 FragColor;

//UNIFORMS
uniform vec3 u_lightColor;
uniform vec3 u_skyColor;
uniform float u_fogDistance;

//FUNCTION PROTOTYPES
float linearize_depth(float depth);

//global variables for fog
float near = 0.1f;
float far = 100.0f;

void main()
{
	//visualizing the depth buffer with foggyness
	float fog_density = u_fogDistance;
	float depth = linearize_depth(gl_FragCoord.z) / far;
	//FragColor = vec4(vec3(depth), 1.0);
	float depth_vec = exp(-pow(depth * fog_density, 2.0));

	//using different fog colors for testing
	vec3 fog_color = u_skyColor;

	//mixing the result with the fog
	vec3 mixed_result = mix(fog_color, u_lightColor, depth_vec);
	FragColor = vec4(mixed_result, 1.0f);
}

float linearize_depth(float depth){
    float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near) );
}