#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

//OUTPUTS
out vec2 textureOutput;
out vec3 normalOutput;
out vec3 fragPosOutput;

//UNIFORMS
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

void main()
{
	//------- phong shading (calculating the lighting in the fragment shader) -------
	textureOutput = aTexCoord;
	fragPosOutput = vec3(u_modelMatrix * vec4(aPos, 1.0f));
	normalOutput = mat3(transpose(inverse(u_modelMatrix))) * aNormal;		//expensive ass shit

	//reading the multiplication from right to left
	gl_Position = u_projectionMatrix * u_viewMatrix * vec4(fragPosOutput, 1.0f);
}