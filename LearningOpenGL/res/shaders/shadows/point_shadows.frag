#version 460 core

out vec4 frag_color;

in VS_OUT {
	vec3 frag_pos;
	vec3 normal;
	vec2 texture_coords;
} fs_input;

uniform sampler2D diffuse;
uniform samplerCube shadow_map; // Now a cubemap.

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 view_pos;

uniform float far_plane; // We also now take the far plane.

// Array of offset direction for sampling.
vec3 grid_sampling_disk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float do_shadow_calculations(vec3 frag_pos);

void main() {
	// Using the blinn-phong lighting from the advanced lighting chapter.
	// We calculate the shadows which is a float that would range from 0.0 and 1.0. 1.0 means
	// the fragment is in the shadow and vice versa.
	vec3 color  = texture(diffuse, fs_input.texture_coords).rgb;
	vec3 normal = normalize(fs_input.normal);

	// Ambient
	vec3 ambient = 0.3 * light_color;

	// Diffuse
	vec3 light_dir = normalize(light_pos - fs_input.frag_pos);
	float diff     = max(dot(light_dir, normal), 0.0);
	vec3 diffuse   = diff * light_color;

	// Specular
	float spec = 0.0;
	vec3 view_dir 	 = normalize(view_pos - fs_input.frag_pos);
	vec3 halfway_dir = normalize(light_dir + view_dir);
	spec = pow(max(dot(normal, halfway_dir), 0.0), 64.0);
	vec3 specular = spec * light_color;

	// 
	// The diffuse and specular are multiplied with the shadow, and since shadows aren't completely
	// dark, we leave the ambient component outside of the shadow multiplications.
	//
	// Essentially, the (1.0 - shadow) identifies which part of the frag_pos is NOT in shadow,
	// therefore the remaining diffuse and specular components get used.
	//
	float shadow  = do_shadow_calculations(fs_input.frag_pos);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	frag_color = vec4(lighting, 1.0f);
}

float do_shadow_calculations(vec3 frag_pos) {
	// Calculate the diff and use it to sample the cubemap.
	vec3 frag_to_light  = frag_pos - light_pos;
	float current_depth = length(frag_to_light);

	float shadow = 0.0;
	float bias   = 0.15;
	int samples  = 20;
	
	float view_distance = length(view_pos - frag_pos);
	float disk_radius   = (1.0 + (view_distance / far_plane)) / 25.0;

	for (int i = 0; i < samples; ++i) {
		float closest_depth = texture(shadow_map, frag_to_light + grid_sampling_disk[i] * disk_radius).r;
		closest_depth *= far_plane; // Undo the [0, 1] mapping.
		if (current_depth - bias > closest_depth)
			shadow += 1.0;
	}

	shadow /= float(samples); // OpenGL doesn't use C-style casts.
	return shadow;
}
