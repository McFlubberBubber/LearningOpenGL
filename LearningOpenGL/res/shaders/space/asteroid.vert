#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 tex_coord;

layout (location = 3) in mat4 instance_matrix;
// @NOTE: When we declare a datatype that is greater than vec4 in the
// vertex attribute, it actually takes up more locations rather than the
// one we assign here. This is because one location can store ONE vec4
// attribute, therefore a mat4 in this case takes up the location of
// 3, 4, 5 and 6.

// Outputs
out VS_OUT {
	vec2 texture_coords;
	vec3 normal;
	vec3 frag_pos;
} vs_out;

// Uniform binding
layout (std140) uniform matrices {
	mat4 projection;
	mat4 view;
};

// uniform mat4 model_matrix;

void main() {
	// Setting up outputs
	vs_out.texture_coords = tex_coord;
	// vs_out.frag_pos		  = vec3(model_matrix * vec4(pos, 1.0f));
	// vs_out.normal		  = mat3(transpose(inverse(model_matrix))) * norm;

	vs_out.frag_pos = vec3(instance_matrix * vec4(pos, 1.0f));
	vs_out.normal   = mat3(transpose(inverse(instance_matrix))) * norm;

	gl_Position = projection * view * vec4(vs_out.frag_pos, 1.0f);
	return;
}
