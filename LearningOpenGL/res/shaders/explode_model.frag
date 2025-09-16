#version 460 core

// @HARDCODE: The number of point lights in the scene are
// hardcoded here in the fragment shader
#define NR_POINT_LIGHTS 4


struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	float shininess;
};


struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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


struct SpotLight {
	vec3 position;
	vec3 direction;

	float constant;
    float linear;
    float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cut_off;
	float outer_cut_off;
};

// Inputs
in GS_OUT {
	vec2 texture_coords;
	vec3 normal;
	vec3 frag_pos;
} fs_in;

// Outputs
out vec4 frag_color;

// Uniforms
uniform vec3 view_position;
uniform vec3 sky_color;
uniform float fog_distance;

uniform Material material;
uniform DirLight dir_light;
uniform PointLight point_lights[NR_POINT_LIGHTS];
uniform SpotLight spot_light;

//Depth buffer stuff
float near = 0.1;
float far = 100.0;


//FUNCTION PROTOTYPES
vec3 calculate_directional_lighting(DirLight dir_light, vec3 norm, vec3 view_direction);

vec3 calculate_point_lighting(PointLight point_lights, vec3 norm, vec3 frag_pos, vec3 view_direction);

vec3 calculate_spot_lighting(SpotLight spot_light, vec3 norm, vec3 frag_pos, vec3 view_direction);

float linearize_depth(float depth);


void main ()
{
	//Calulating lighting properties (PHONG SHADING)
	vec3 norm = normalize(fs_in.normal);
	vec3 view_direction  = normalize(view_position - fs_in.frag_pos);

	//Phase 1: Directional Lighting
	vec3 result = calculate_directional_lighting(dir_light, norm, view_direction);

	//Phase 2: Point Lighting
	for (int i = 0; i < NR_POINT_LIGHTS; i++){
		result += calculate_point_lighting(point_lights[i], norm, fs_in.frag_pos, view_direction);
	}

	//Phase 3: Spot Lighting
	result += calculate_spot_lighting(spot_light, norm, fs_in.frag_pos, view_direction);
	
	//visualizing the depth buffer with foggyness
	float fog_density = fog_distance;
	float depth = linearize_depth(gl_FragCoord.z) / far;
	float depth_vec = exp(-pow(depth * fog_density, 2.0));

	//using different fog colors for testing
	vec3 fog_color = sky_color;

	//mixing the result with the fog
	vec3 mixed_result = mix(fog_color, result, depth_vec);
	frag_color = vec4(mixed_result, 1.0f);
	
	// Outputting a different texture on the inside of the container
	// using the gl_FrontFacing bool value
/*
	if (gl_FrontFacing)
		frag_color = vec4(mixed_result, 1.0f);
	else
		frag_color = texture(material.diffuse2, fs_in.texture_coords);
*/
}


//for calculating any directional lighting in the scene
vec3 calculate_directional_lighting(DirLight dir_light, vec3 norm, vec3 view_direction){
	//getting light direction using the direction
	vec3 light_direction = normalize(-dir_light.direction);											//normalizing the negative of dirLight's direction attribute
	
	//diffuse 
	float diff = max(dot(norm, light_direction), 0.0f);													//calculating diffuse with dot product of normals and lightDirection
	
	//specular
	vec3 reflect_direction = reflect(-light_direction, norm);												//getting the reflect direction based on the negative lightDirection and the normals
	
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess);			//calculating specular with power based on shininess, dot prod on view + ref directions

	//combining results
	vec3 ambient = dir_light.ambient * vec3(texture(material.diffuse1, fs_in.texture_coords));				//light ambient multiplied with material diffuse's texture

	vec3 diffuse = dir_light.diffuse * diff * vec3(texture(material.diffuse1, fs_in.texture_coords));		//light diffuse multiplied with material diffuse's texture

	vec3 specular = dir_light.specular * spec * vec3(texture(material.specular1, fs_in.texture_coords));		//light specular multiplied with material specular's texture

	//returning vec3 result
	return (ambient + diffuse + specular);
}


//for calculating any number of point lights that can exist within the scene
vec3 calculate_point_lighting(PointLight point_lights, vec3 norm, vec3 frag_pos, vec3 view_direction) {
	//getting light direction using the position
	vec3 light_direction = normalize(point_lights.position - frag_pos);

	//diffuse
	float diff = max(dot(norm, light_direction), 0.0f);

	//specular
	vec3 reflect_direction = reflect(-light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess);

	//attenuation
	float distance = length(point_lights.position - frag_pos);
	float attenuation = 1.0f / (point_lights.constant + point_lights.linear * distance + point_lights.quadratic * (distance * distance));

	//combining results
	vec3 ambient = point_lights.ambient * vec3(texture(material.diffuse1, fs_in.texture_coords));

	vec3 diffuse = point_lights.diffuse * diff * vec3(texture(material.diffuse1, fs_in.texture_coords));

	vec3 specular = point_lights.specular * spec * vec3(texture(material.specular1, fs_in.texture_coords));

	//applying attenuation to lighting vectors
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calculate_spot_lighting(SpotLight spot_light, vec3 norm, vec3 frag_pos, vec3 view_direction) {
	//getting the light direction by using the position of the player
	vec3 light_direction = normalize(spot_light.position - frag_pos);

	//diffuse
	float diff = max(dot(norm, light_direction), 0.0f);
	//specular
	vec3 reflect_direction = reflect(-light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess);

	//attenuation
	float distance = length(spot_light.position - frag_pos);
	float attenuation = 1.0f / (spot_light.constant + spot_light.linear * distance + spot_light.quadratic * (distance * distance));

	//intensity
	float theta = dot(light_direction, normalize(-spot_light.direction));
	float epsilon = spot_light.cut_off - spot_light.outer_cut_off;
	float intensity = clamp((theta - spot_light.outer_cut_off) / epsilon, 0.0f, 1.0f);		//clamping the values between 0 and 1

	//applying spotlight
	vec3 ambient = spot_light.ambient * vec3(texture(material.diffuse1, fs_in.texture_coords));

	vec3 diffuse = spot_light.diffuse * diff * vec3(texture(material.diffuse1, fs_in.texture_coords));

	vec3 specular = spot_light.specular * spec * vec3(texture(material.specular1, fs_in.texture_coords));

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}

float linearize_depth(float depth){
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near) );
}










