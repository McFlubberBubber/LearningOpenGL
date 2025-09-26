#version 460 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4f;

layout (std140) uniform matrices {
	mat4 projection;
	mat4 view;
};

void generate_line(int index) {
	gl_Position = projection * gl_in[index].gl_Position;
	EmitVertex();

	gl_Position = projection * (gl_in[index].gl_Position +
								vec4(gs_in[index].normal, 0.0f) * MAGNITUDE);
	EmitVertex();

	EndPrimitive();
}

void main() {
	generate_line(0);
	generate_line(1);
	generate_line(2);

	return;
}
