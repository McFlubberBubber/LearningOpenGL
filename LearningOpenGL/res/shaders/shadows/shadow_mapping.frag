#version 460 core
out vec4 frag_color;

in VS_OUT {
	vec3 frag_pos;
	vec3 normal;
	vec2 texture_coords;
	vec4 frag_pos_light_space;
} fs_input;

uniform sampler2D diffuse;
uniform sampler2D shadow_map;

uniform vec3 light_pos;
uniform vec3 view_pos;

float do_shadow_calculations(vec4 fpls); // fpls = frag pos light space. 

void main() {
	// Using the blinn-phong lighting from the advanced lighting chapter.
	// We calculate the shadows which is a float that would range from 0.0 and 1.0. 1.0 means
	// the fragment is in the shadow and vice versa.
	vec3 color  = texture(diffuse, fs_input.texture_coords).rgb;
	vec3 normal = normalize(fs_input.normal);
	vec3 light_color = vec3(1.0f);

	// Ambient
	vec3 ambient = 0.15 * light_color;

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
	float shadow  = do_shadow_calculations(fs_input.frag_pos_light_space);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	frag_color = vec4(lighting, 1.0f);
}

float do_shadow_calculations(vec4 fpls) {
	// First, perform perspective divide to get normalized device coords (returns [-1, 1]).
	vec3 proj_coords = fpls.xyz / fpls.w;

	// Then, transform the NDC coords to [0, 1].
	proj_coords = proj_coords * 0.5 + 0.5;

	// Check if fragment is outside the light's frustum.
    if (proj_coords.z > 1.0) return 0.0;  // Beyond far plane
    if (proj_coords.z < 0.0) return 0.0;  // Beyond near plane (behind light)
    
    if (proj_coords.x < 0.0 || proj_coords.x > 1.0 ||
        proj_coords.y < 0.0 || proj_coords.y > 1.0) {
        return 0.0;  // Outside frustum bounds - not in shadow
    }

	float closest_depth = texture(shadow_map, proj_coords.xy).r;
	float current_depth = proj_coords.z;

	vec3 normal = normalize(fs_input.normal);
	vec3 light_dir = normalize(light_pos - fs_input.frag_pos);
	float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);

	// Now, just check if the current_depth is higher than the closest_depth to know whether a
	// given fragment is within a shadow or not.
	float shadow = 0.0;

	// We use PCF (percentage-closer filtering) to soften the shadows by making them appear
	// less blocky, therefore we just take the surrounding texels of the depth map and average them
	// to make them kinda blurry.
	vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r;
			shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;
	return shadow;
}
