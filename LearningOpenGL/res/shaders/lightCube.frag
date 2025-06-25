#version 330 core
//OUTPUTS
out vec4 FragColor;

//UNIFORMS
uniform vec3 u_lightColor;

//FUNCTION PROTOTYPES
float LinearizeDepth(float depth);

//global variables for fog
float near = 0.1f;
float far = 100.0f;

void main()
{

    //without fog effect
    //FragColor = vec4(u_lightColor, 1.0f); // setting values based on uniform
   

    //visualizing the depth buffer with foggyness
	float fogDensity = 5.0f;
	float depth = LinearizeDepth(gl_FragCoord.z) / far;
	//FragColor = vec4(vec3(depth), 1.0);
	float depthVec = exp(-pow(depth * fogDensity, 2.0));

	//using different fog colors for testing
	vec3 fogColor = vec3(0.001f, 0.001f, 0.001f);			//set to BG color
	//vec3 fogColor = vec3(1.0, 0.0, 0.0);					//bright red

	//mixing the result with the fog
	vec3 mixedResult = mix(fogColor, u_lightColor, depthVec);
	FragColor = vec4(mixedResult, 1.0f);
}

float LinearizeDepth(float depth){
    float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near) );
}