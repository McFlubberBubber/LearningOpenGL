#version 330 core
//material properties
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

//lighting properties
struct Light {
	//vec3 position;
	vec3 direction;			//for directional lighting
	vec3 ambient;
	vec3 diffuse; 
	vec3 specular;
};

//INPUTS
in vec3 normalOutput;
in vec3 fragPosOutput;
in vec2 textureOutput;

//OUTPUTS
out vec4 FragColor;

//UNIFORMS
uniform Material material;
uniform Light light;
uniform vec3 u_viewPosition;

void main ()
{
	//------ PHONG SHADING ------

	//ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, textureOutput));

	//diffuse
	vec3 norm = normalize(normalOutput);
	//vec3 lightDirecton = normalize(light.position - fragPosOutput);		//for when using positional lighting
	vec3 lightDirecton = normalize(-light.direction);						//for using directional lighting
	float diff = max(dot(norm, lightDirecton), 0.0f);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, textureOutput));

	//specular
	vec3 viewDirection = normalize(u_viewPosition - fragPosOutput);
	vec3 reflectDirection = reflect(-lightDirecton, norm);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, textureOutput));

	//emission
	vec3 emission = vec3(0.0f);
	if (texture(material.specular, textureOutput).r == 0.0f)		//if the specularMap has values of 0.0f, apply the emissionMap
	{
		emission = texture(material.emission, textureOutput).rgb;
	}

	vec3 result = ambient + diffuse + specular + emission;
	FragColor = vec4(result, 1.0);
}