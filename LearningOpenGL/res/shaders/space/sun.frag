#version 460 core
//OUTPUTS
out vec4 frag_color;

//UNIFORMS
uniform vec3 light_color;

void main()
{
	frag_color = vec4(light_color, 1.0f);
}


