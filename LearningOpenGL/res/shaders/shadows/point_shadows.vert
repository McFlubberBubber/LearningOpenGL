#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 tex_coords;

out vec2 texture_output;

// @NOTE: There is no need to output the frag_pos_light_space since we
// can sample the depth values with a dir vector.
out VS_OUT {
	vec3 frag_pos;
	vec3 normal;
	vec2 texture_coords;
} vs_output;

// Uniforms binding.
layout (std140) uniform matrices {
	mat4 projection;
	mat4 view;
};

uniform mat4 model_matrix;

void main() {
	vs_output.frag_pos 		 = vec3(model_matrix * vec4(pos, 1.0));
	vs_output.normal 		 = transpose(inverse(mat3(model_matrix))) * norm;
	vs_output.texture_coords = tex_coords;

	gl_Position = projection * view * model_matrix * vec4(pos, 1.0);
}
