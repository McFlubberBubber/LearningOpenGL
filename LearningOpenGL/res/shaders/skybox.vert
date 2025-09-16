#version 460 core

layout (location = 0) in vec3 pos;

out vec3 texture_output;

// These are not the same as the stuff that is used in the unform buffer since we are manually
// adjusting the view_matrix when we call the draw_skybox().
uniform mat4 projection;
uniform mat4 view;


// The difference here compared to other vertex shaders
// is that we set the incoming positon vector to the out
// going textureOutput within the main function
void main() {
	texture_output = pos;
/*
	vec4 pos = u_projectionMatrix * u_viewMatrix * vec4(aPos, 1.0f);
	gl_Position = pos.xyww;
*/

	gl_Position = projection * view * vec4(pos, 1.0f);
}
