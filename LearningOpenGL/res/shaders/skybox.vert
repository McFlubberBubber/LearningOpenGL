#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 textureOutput;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;


// The difference here compared to other vertex shaders
// is that we set the incoming positon vector to the out
// going textureOutput within the main function
void main() {
	textureOutput = aPos;

/*
	vec4 pos = u_projectionMatrix * u_viewMatrix * vec4(aPos, 1.0f);
	gl_Position = pos.xyww;
*/

	gl_Position = u_projectionMatrix * u_viewMatrix * vec4(aPos, 1.0f);


}
