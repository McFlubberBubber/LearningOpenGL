#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// INPUT BLOCK
in VS_OUT {
	vec2 texture_coords;
	vec3 normal;
	vec3 frag_pos;
} gs_in[];

// OUTPUT
out GS_OUT {
	vec2 texture_coords;
	vec3 normal;
	vec3 frag_pos;
} gs_out;

// UNIFORMS
layout (std140) uniform matrices {
	mat4 projection;
	mat4 view;
};
uniform float time;

// Function prototypes
vec3 get_normal();
vec4 explode_model(int vertex_index, vec3 normal);

void main() {
	vec3 normal = get_normal();

//	gl_Position = explode_model(gl_in[0].gl_Position, normal);
	gl_Position = explode_model(0, normal);
	gs_out.texture_coords = gs_in[0].texture_coords;
	gs_out.normal = gs_in[0].normal;
	gs_out.frag_pos = gs_in[0].frag_pos;
	EmitVertex();

//	gl_Position = explode_model(gl_in[1].gl_Position, normal);
	gl_Position = explode_model(1, normal);
	gs_out.texture_coords = gs_in[1].texture_coords;
	gs_out.normal = gs_in[1].normal;
	gs_out.frag_pos = gs_in[1].frag_pos;
	EmitVertex();

//	gl_Position = explode_model(gl_in[2].gl_Position, normal);
	gl_Position = explode_model(2, normal);
	gs_out.texture_coords = gs_in[2].texture_coords;
	gs_out.normal = gs_in[2].normal;
	gs_out.frag_pos = gs_in[2].frag_pos;
	EmitVertex();

	EndPrimitive();
}


// Getting the normal vector 
vec3 get_normal() {
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));		// Order is important since if we switch it, the normal would be facing in the opposite direction.
}

vec4 explode_model(int vertex_index, vec3 normal) {
	// Getting the world position.
	vec3 world_pos = gs_in[vertex_index].frag_pos;

	// Applying the "explosion".
	float magnitude = 1.25f;
	vec3 direction = normal * ((sin(time) + 1.0f) / 2.0f) * magnitude;
	world_pos += direction;

	// Transforming it back into clip space.
	return projection * view * vec4(world_pos, 1.0f);
}
