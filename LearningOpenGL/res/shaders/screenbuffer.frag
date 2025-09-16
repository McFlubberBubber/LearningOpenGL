#version 460 core

//INPUTS
in vec2 texture_output;

//OUTPUTS
out vec4 frag_color;

//UNIFORMS
uniform sampler2D screen_texture;		// Initialized to 0 already.
uniform int render_mode;

//VARIBLES
const float offset = 1.0f / 300.0f;		// Configurable to our liking

//POST PROCESSING MODES
const int NORMAL_MODE	    = 0;
const int INVERT_MODE	    = 1;
const int GRAYSCALE_MODE    = 2;
const int SHARPEN_MODE	    = 3;
const int DARK_SHARPEN_MODE	= 4;

//FUNCTION PROTOTYPES
vec4 process_sharpening(bool do_dark_sharpening);

void main() {
	vec4 color = texture(screen_texture, texture_output);

	switch (render_mode) {
		case INVERT_MODE:
			color.rgb = 1.0f - color.rgb;
			break;

		case GRAYSCALE_MODE:
			float k = dot(color.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
			color = vec4(vec3(k), 1.0f);
			break;

		case SHARPEN_MODE:
			color = process_sharpening(false);
			break;

		case DARK_SHARPEN_MODE:
			color = process_sharpening(true);
			break;
			
		default: //NORMAL_MODE
			break;
	}

	frag_color = color;
}

vec4 process_sharpening (bool do_dark_sharpening) {
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

	float kernal[9];
	if (!do_dark_sharpening) {
		kernal[0] = -1; kernal[1] = -1; kernal[2] = -1;
		kernal[3] = -1; kernal[4] =  9; kernal[5] = -1;
		kernal[6] = -1; kernal[7] = -1; kernal[8] = -1;
		
	} else {
		kernal[0] =  1; kernal[1] =  1; kernal[2] =  1;
		kernal[3] =  1; kernal[4] = -8; kernal[5] =  1;
		kernal[6] =  1; kernal[7] =  1; kernal[8] =  1;
	}


	vec3 acc = vec3(0.0f);
	for (int i = 0; i < 9; i++) {
		acc += vec3(texture(screen_texture, texture_output + offsets[i])) * kernal[i];
	}

	return vec4(acc, 1.0f);
}