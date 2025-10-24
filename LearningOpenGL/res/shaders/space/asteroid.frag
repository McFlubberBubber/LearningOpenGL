#version 460 core

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;

	float shininess;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// Inputs
in VS_OUT {
	in vec2 texture_coords;
	vec3 normal;
	vec3 frag_pos;
} fs_in;

// Outputs
out vec4 frag_color;

// Uniforms
uniform vec3 view_position;

uniform Material material;
uniform PointLight sunlight;


// Function prototypes
vec3 calculate_point_lighting(PointLight sunlight, vec3 norm, vec3 frag_pos, vec3 view_direction);

void main() {
	// Calculating sun light
	vec3 norm			= normalize(fs_in.normal);
	vec3 view_direction = normalize(view_position - fs_in.frag_pos);
	vec3 result = calculate_point_lighting(sunlight, norm, fs_in.frag_pos, view_direction);

	frag_color = vec4(result, 1.0f);

	return;
}

/*
vec3 calculate_point_lighting(PointLight sunlight, vec3 norm, vec3 frag_pos, vec3 view_direction) {
	//getting light direction using the position
	vec3 light_direction = normalize(sunlight.position - frag_pos);

	//diffuse
	float diff = max(dot(norm, light_direction), 0.0f);

	//specular
	vec3 reflect_direction = reflect(-light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess);

	//attenuation
	float distance = length(sunlight.position - frag_pos);
	float attenuation = 1.0f / (sunlight.constant + sunlight.linear * distance + sunlight.quadratic * (distance * distance));

	//combining results
	vec3 ambient = sunlight.ambient * vec3(texture(material.diffuse1, fs_in.texture_coords));
	vec3 diffuse = sunlight.diffuse * diff * vec3(texture(material.diffuse1, fs_in.texture_coords));
	vec3 specular = sunlight.specular * spec * vec3(texture(material.specular1, fs_in.texture_coords));

	//applying attenuation to lighting vectors
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}
*/

vec3 calculate_point_lighting(PointLight sunlight, vec3 norm, vec3 frag_pos, vec3 view_direction) {
	//getting light direction using the position
	vec3 light_direction = normalize(sunlight.position - frag_pos);

	//diffuse
	float diff = max(dot(norm, light_direction), 0.0f);

	//specular
	vec3 reflect_direction = reflect(-light_direction, norm);
	vec3 halfway_direction = normalize(light_direction + view_direction);
	// float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	float spec = pow(max(dot(norm, halfway_direction), 0.0), material.shininess);


	//attenuation
	float distance = length(sunlight.position - frag_pos);
	float attenuation = 1.0f / (sunlight.constant + sunlight.linear * distance + sunlight.quadratic * (distance * distance));

	//combining results
	vec3 ambient = sunlight.ambient * vec3(texture(material.diffuse1, fs_in.texture_coords));

	vec3 diffuse = sunlight.diffuse * diff * vec3(texture(material.diffuse1, fs_in.texture_coords));

	vec3 specular = sunlight.specular * spec * vec3(texture(material.specular1, fs_in.texture_coords));

	//applying attenuation to lighting vectors
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}
