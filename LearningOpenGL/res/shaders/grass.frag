#version 460 core

//defining how many point lights will exist in the scene
#define NR_POINT_LIGHTS 4

//material properties
struct Material {
	sampler2D textureDiffuse1;
	sampler2D textureSpecular1;
	float shininess;
};

//Directional light properties
struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

//Point light properties
struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

//Spot light properties
struct SpotLight {
	vec3 position;
	vec3 direction;

	float constant;
    float linear;
    float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cutOff;
	float outerCutOff;
};


// Inputs
in VS_OUT {
	vec2 texture_coords;
	vec3 normal;
	vec3 frag_pos;
} fs_input;


//OUTPUTS
out vec4 FragColor;

//UNIFORMS
uniform vec3 u_viewPosition;
uniform vec3 u_skyColor;
uniform float u_fogDistance;
uniform float u_textureTiling;							//scaling how the texture is

uniform Material u_material;
uniform DirLight u_dirLight;
uniform PointLight u_pointLight[NR_POINT_LIGHTS];		//based on the definition of point lights 
uniform SpotLight u_spotLight;


//Depth buffer stuff
float near = 0.1;
float far = 100.0;


//FUNCTION PROTOTYPES
vec3 calculate_directional_lighting(DirLight u_dirLight, vec3 norm, vec3 view_direction);

vec3 calculate_point_lighting(PointLight u_pointLight, vec3 norm, vec3 frag_pos, vec3 view_direction);

vec3 calculate_spot_lighting(SpotLight u_spotLight, vec3 norm, vec3 frag_pos, vec3 view_direction);

float linearize_depth(float depth);



void main ()
{
	//Calulating lighting properties (PHONG SHADING)
	vec3 norm = normalize(fs_input.normal);
	vec3 view_direction  = normalize(u_viewPosition - fs_input.frag_pos);

	//Phase 1: Directional Lighting
	vec3 result = calculate_directional_lighting(u_dirLight, norm, view_direction);

	//Phase 2: Point Lighting
	for (int i = 0; i < NR_POINT_LIGHTS; i++){
		result += calculate_point_lighting(u_pointLight[i], norm, fs_input.frag_pos, view_direction);
	}

	//Phase 3: Spot Lighting
	result += calculate_spot_lighting(u_spotLight, norm, fs_input.frag_pos, view_direction);

	//Discarding transparent stuff
	vec4 texture_color = texture(u_material.textureDiffuse1, fs_input.texture_coords);
	if(texture_color.a < 0.1f)
		discard;

	//Applying lighting to ONLY the RGB values of the textureColor
	vec3 lit_color = result * texture_color.rgb;


	//visualizing the depth buffer with foggyness
	float fog_density = u_fogDistance;
	float depth = linearize_depth(gl_FragCoord.z) / far;
	//FragColor = vec4(vec3(depth), 1.0);
	float depth_vec = exp(-pow(depth * fog_density, 2.0));

	//using different fog colors for testing
	vec3 fog_color = u_skyColor;


	//mixing the result with the fog
	vec3 mixed_result = mix(fog_color, lit_color, depth_vec);
	FragColor = vec4(mixed_result, texture_color.a);
}



//for calculating any directional lighting in the scene
vec3 calculate_directional_lighting(DirLight u_dirLight, vec3 norm, vec3 view_direction){
	//getting light direction using the direction
	vec3 light_direction = normalize(-u_dirLight.direction);											//normalizing the negative of dirLight's direction attribute
	
	//diffuse 
	float diff = max(dot(norm, light_direction), 0.0f);													//calculating diffuse with dot product of normals and lightDirection
	
	//specular
	vec3 reflect_direction = reflect(-light_direction, norm);												//getting the reflect direction based on the negative lightDirection and the normals
	
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), u_material.shininess);			//calculating specular with power based on shininess, dot prod on view + ref directions

	//combining results
	vec3 ambient = u_dirLight.ambient * vec3(texture(u_material.textureDiffuse1, fs_input.texture_coords));				//light ambient multiplied with material diffuse's texture

	vec3 diffuse = u_dirLight.diffuse * diff * vec3(texture(u_material.textureDiffuse1, fs_input.texture_coords));		//light diffuse multiplied with material diffuse's texture

	vec3 specular = u_dirLight.specular * spec * vec3(texture(u_material.textureSpecular1, fs_input.texture_coords));		//light specular multiplied with material specular's texture

	//returning vec3 result
	return (ambient + diffuse + specular);
}


//for calculating any number of point lights that can exist within the scene
vec3 calculate_point_lighting(PointLight u_pointLight, vec3 norm, vec3 frag_pos, vec3 view_direction) {
	//getting light direction using the position
	vec3 light_direction = normalize(u_pointLight.position - frag_pos);

	//diffuse
	float diff = max(dot(norm, light_direction), 0.0f);

	//specular
	vec3 reflect_direction = reflect(-light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), u_material.shininess);

	//attenuation
	float distance = length(u_pointLight.position - frag_pos);
	float attenuation = 1.0f / (u_pointLight.constant + u_pointLight.linear * distance + u_pointLight.quadratic * (distance * distance));

	//combining results
	vec3 ambient = u_pointLight.ambient * vec3(texture(u_material.textureDiffuse1, fs_input.texture_coords));

	vec3 diffuse = u_pointLight.diffuse * diff * vec3(texture(u_material.textureDiffuse1, fs_input.texture_coords));

	vec3 specular = u_pointLight.specular * spec * vec3(texture(u_material.textureSpecular1, fs_input.texture_coords));

	//applying attenuation to lighting vectors
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calculate_spot_lighting(SpotLight u_spotLight, vec3 norm, vec3 frag_pos, vec3 view_direction) {
	//getting the light direction by using the position of the player
	vec3 light_direction = normalize(u_spotLight.position - frag_pos);

	//diffuse
	float diff = max(dot(norm, light_direction), 0.0f);
	//specular
	vec3 reflect_direction = reflect(-light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), u_material.shininess);

	//attenuation
	float distance = length(u_spotLight.position - frag_pos);
	float attenuation = 1.0f / (u_spotLight.constant + u_spotLight.linear * distance + u_spotLight.quadratic * (distance * distance));

	//intensity
	float theta = dot(light_direction, normalize(-u_spotLight.direction));
	float epsilon = u_spotLight.cutOff - u_spotLight.outerCutOff;
	float intensity = clamp((theta - u_spotLight.outerCutOff) / epsilon, 0.0f, 1.0f);		//clamping the values between 0 and 1

	//applying spotlight
	vec3 ambient = u_spotLight.ambient * vec3(texture(u_material.textureDiffuse1, fs_input.texture_coords));

	vec3 diffuse = u_spotLight.diffuse * diff * vec3(texture(u_material.textureDiffuse1, fs_input.texture_coords));

	vec3 specular = u_spotLight.specular * spec * vec3(texture(u_material.textureSpecular1, fs_input.texture_coords));

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}

float linearize_depth(float depth){
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near) );
}