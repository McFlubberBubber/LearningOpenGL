#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 tex_coord;

// Outputs
out VS_OUT {
	vec2 texture_coords;
	vec3 normal;
	vec3 frag_pos;
} vs_out;


// Uniforms
layout (std140) uniform matrices {
	mat4 projection;
	mat4 view;
};

uniform mat4 model_matrix;


void main()
{
	// Phong Lighting
	vs_out.texture_coords = tex_coord;
	vs_out.frag_pos = vec3(model_matrix * vec4(pos, 1.0f));
	vs_out.normal = mat3(transpose(inverse(model_matrix))) * norm;		//expensive ass shit

	//reading the multiplication from right to left
	gl_Position = projection * view * vec4(vs_out.frag_pos, 1.0f);
}
