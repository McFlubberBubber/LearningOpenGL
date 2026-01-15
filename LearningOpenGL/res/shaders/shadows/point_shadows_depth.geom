#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out; // Output 6 triangles (18 verts).

uniform mat4 shadow_matrices[6];
uniform mat4 model_matrix;

out vec4 frag_pos;

void main() {
	for (int face = 0; face < 6; ++face) {
		gl_Layer = face; // Specified which face we render.

		// Looping for each triangle vertex.
		for (int i = 0; i < 3; ++i) {
			frag_pos = gl_in[i].gl_Position;
			gl_Position = shadow_matrices[face] * frag_pos;

			// vec4 world_pos = model_matrix * gl_in[i].gl_Position;
			// frag_pos = world_pos;
			// gl_Position = shadow_matrices[face] * world_pos;
			EmitVertex();
		}
		
		EndPrimitive();
	}
}
