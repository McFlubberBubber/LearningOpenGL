#version 460 core

in vec4 frag_pos; // From the geometry shader stage.

uniform vec3 light_pos;
uniform float far_plane;

void main() {
	// Get the distance between the fragment and the light source.
	float d = length(frag_pos.xyz - light_pos);

	// Map to [0, 1] by dividing the far_plane.
	d = d / far_plane;

	// Write the value as the modified depth.
	gl_FragDepth = d;
}
