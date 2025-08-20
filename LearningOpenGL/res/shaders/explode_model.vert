#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// Outputs
out VS_OUT {
	vec2 texture_coords;
	vec3 normal;
	vec3 frag_pos;
} vs_output;


// Uniforms
layout (std140) uniform u_matrices {
	mat4 projection;
	mat4 view;
};

uniform mat4 u_modelMatrix;


void main()
{
	// Phong Lighting
	vs_output.texture_coords = aTexCoord;
	vs_output.frag_pos = vec3(u_modelMatrix * vec4(aPos, 1.0f));
	vs_output.normal = mat3(transpose(inverse(u_modelMatrix))) * aNormal;		//expensive ass shit

	//reading the multiplication from right to left
	gl_Position = projection * view * vec4(vs_output.frag_pos, 1.0f);
}
