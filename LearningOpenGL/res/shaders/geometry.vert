#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

// OUTPUTS
out VS_OUT {
	vec3 color;
} vs_out;


void main() {
	// Drawing points on the z-plane.
	gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
	vs_out.color = aColor;
}
