
#version 330 core

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

//INPUTS
in vec3 normalOutput;
in vec3 fragPosOutput;
in vec2 textureOutput;

//OUTPUTS
out vec4 FragColor;

//UNIFORMS
uniform vec3 u_viewPosition;
uniform Material u_material;
uniform DirLight u_dirLight;
uniform PointLight u_pointLight[NR_POINT_LIGHTS];		//based on the definition of point lights 
uniform SpotLight u_spotLight;

//FUNCTION PROTOTYPES
vec3 CalculateDirectionalLight(DirLight u_dirLight, vec3 norm, vec3 viewDirection);
vec3 CalculatePointLight(PointLight u_pointLight, vec3 norm, vec3 fragPosOutput, vec3 viewDirection);
vec3 CaluclateSpotLight(SpotLight u_spotLight, vec3 norm, vec3 fragPosOutput, vec3 viewDirection);

void main ()
{
	//Calulating lighting properties (PHONG SHADING)
	vec3 norm = normalize(normalOutput);
	vec3 viewDirection = normalize(u_viewPosition - fragPosOutput);

	//Phase 1: Directional Lighting
	vec3 result = CalculateDirectionalLight(u_dirLight, norm, viewDirection);

	//Phase 2: Point Lighting
	for (int i = 0; i < NR_POINT_LIGHTS; i++){
		result += CalculatePointLight(u_pointLight[i], norm, fragPosOutput, viewDirection);
	}

	//Phase 3: Spot Lighting
	result += CaluclateSpotLight(u_spotLight, norm, fragPosOutput, viewDirection);

	//Applying all lighting calculations
	FragColor = vec4(result, 1.0);
}



//for calculating any directional lighting in the scene
vec3 CalculateDirectionalLight(DirLight u_dirLight, vec3 norm, vec3 viewDirection){
	//getting light direction using the direction
	vec3 lightDirecton = normalize(-u_dirLight.direction);												//normalizing the negative of dirLight's direction attribute
	
	//diffuse 
	float diff = max(dot(norm, lightDirecton), 0.0f);													//calculating diffuse with dot product of normals and lightDirection
	//specular
	vec3 reflectDirection = reflect(-lightDirecton, norm);												//getting the reflect direction based on the negative lightDirection and the normals
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), u_material.shininess);			//calculating specular with power based on shininess, dot prod on view + ref directions

	//combining results
	vec3 ambient = u_dirLight.ambient * vec3(texture(u_material.textureDiffuse1, textureOutput));				//light ambient multiplied with material diffuse's texture
	vec3 diffuse = u_dirLight.diffuse * diff * vec3(texture(u_material.textureDiffuse1, textureOutput));		//light diffuse multiplied with material diffuse's texture
	vec3 specular = u_dirLight.specular * spec * vec3(texture(u_material.textureSpecular1, textureOutput));		//light specular multiplied with material specular's texture

	//returning vec3 result
	return (ambient + diffuse + specular);
}


//for calculating any number of point lights that can exist within the scene
vec3 CalculatePointLight(PointLight u_pointLight, vec3 norm, vec3 fragPosOutput, vec3 viewDirection) {
	//getting light direction using the position
	vec3 lightDirecton = normalize(u_pointLight.position - fragPosOutput);

	//diffuse
	float diff = max(dot(norm, lightDirecton), 0.0f);

	//specular
	vec3 reflectDirection = reflect(-lightDirecton, norm);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), u_material.shininess);

	//attenuation
	float distance = length(u_pointLight.position - fragPosOutput);
	float attenuation = 1.0f / (u_pointLight.constant + u_pointLight.linear * distance + u_pointLight.quadratic * (distance * distance));

	//combining results
	vec3 ambient = u_pointLight.ambient * vec3(texture(u_material.textureDiffuse1, textureOutput));
	vec3 diffuse = u_pointLight.diffuse * diff * vec3(texture(u_material.textureDiffuse1, textureOutput));
	vec3 specular = u_pointLight.specular * spec * vec3(texture(u_material.textureSpecular1, textureOutput));

	//applying attenuation to lighting vectors
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 CaluclateSpotLight(SpotLight u_spotLight, vec3 norm, vec3 fragPosOutput, vec3 viewDirection) {
	//getting the light direction by using the position of the player
	vec3 lightDirecton = normalize(u_spotLight.position - fragPosOutput);

	//diffuse
	float diff = max(dot(norm, lightDirecton), 0.0f);
	//specular
	vec3 reflectDirection = reflect(-lightDirecton, norm);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), u_material.shininess);

	//attenuation
	float distance = length(u_spotLight.position - fragPosOutput);
	float attenuation = 1.0f / (u_spotLight.constant + u_spotLight.linear * distance + u_spotLight.quadratic * (distance * distance));

	//intensity
	float theta = dot(lightDirecton, normalize(-u_spotLight.direction));
	float epsilon = u_spotLight.cutOff - u_spotLight.outerCutOff;
	float intensity = clamp((theta - u_spotLight.outerCutOff) / epsilon, 0.0f, 1.0f);		//clamping the values between 0 and 1

	//applying spotlight
	vec3 ambient = u_spotLight.ambient * vec3(texture(u_material.textureDiffuse1, textureOutput));
	vec3 diffuse = u_spotLight.diffuse * diff * vec3(texture(u_material.textureDiffuse1, textureOutput));
	vec3 specular = u_spotLight.specular * spec * vec3(texture(u_material.textureSpecular1, textureOutput));

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}