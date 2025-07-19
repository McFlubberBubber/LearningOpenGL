#version 330 core

//INPUTS
in vec2 textureOutput;

//OUTPUTS
out vec4 FragColor;

//UNIFORMS
uniform sampler2D u_screenTexture;
uniform bool u_invertFilter;

//VARIBLES
const float offset = 1.0f / 300.0f;		// Configurable to our liking


// @HARDCODE All the sections of code that appear under this main
// function are not toggle-able during the render of the application.
// These functionalities depend on the new user input system to be
// implemented into the project before we can work on toggle modes
// for each post processing effect.
void main() {

	// @TODO This block of code relies on the 'toggleInvert()' function
	// that has not been programmed into the application yet.
	// POST PROCESSING: Inverting the colors rendered on the scene
	/*
	if (u_invertFilter) {
		FragColor = vec4(vec3(1.0f - texture(u_screenTexture, textureOutput)), 1.0f);
	} else {
		FragColor = texture(u_screenTexture, textureOutput);		
	}
	*/


	// POST PROCESSING: Rendering the scene on a gray scale	
	// Variables for gray scaling
	float redScale   = 0.2126f;
	float greenScale = 0.7152f;
	float blueScale	 = 0.0722f;

	// This is the default screen buffer rendering
	FragColor = texture(u_screenTexture, textureOutput);		

	float grayScale = FragColor.r * redScale + FragColor.g * greenScale + FragColor.b * blueScale;
	FragColor = vec4(grayScale, grayScale, grayScale, 1.0f);


	// POST PROCESSING: Using kernals and stuff
	vec2 offsets[9] = vec2[] (
		vec2(-offset,	offset),		// top-left
		vec2(0.0f,		offset),		// top-center
		vec2(offset,	offset),		// top-right
		vec2(0.0f,		-offset),		// center-left
		vec2(0.0f,		0.0f),			// center  
		vec2(offset,	0.0f),			// center-right
		vec2(-offset,	-offset),		// bottom-left
		vec2(0.0f,		-offset),		// bottom-center
		vec2(offset,	-offset)		// bottom-right
	);


	// Normal sharpening
	float kernal[9] = float[] (
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	);


	// Dark sharperning
	/*
	float kernal[9] = float[] (
		1,  1, 1,
		1, -8, 1,
		1,  1, 1
	);
	*/

	vec3 sampleTex[9];
	for (int i = 0; i < 9; i++) {
		sampleTex[i] = vec3(texture(u_screenTexture, textureOutput.st + offsets[i]));
	}
	
	vec3 color = vec3(0.0f);
	for (int i = 0; i < 9; i++) {
		color += sampleTex[i] * kernal[i];
	}

	FragColor = vec4(color, 1.0f);
}

