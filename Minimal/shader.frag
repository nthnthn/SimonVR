#version 330 core

// This is a sample fragment shader.

uniform struct Material {
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

uniform struct DirLight {
	int isOn;
	vec3 dir;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} dirLight;

uniform struct PointLight {
    int isOn;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} pointLight;


in vec3 fragPos;
in vec3 norm;
in vec3 pos;

out vec4 outColor;

vec3 calcDirLight(DirLight, vec3, vec3);
vec3 calcPointLight(PointLight, vec3, vec3, vec3);

void main()
{
	vec3 color;
	vec3 viewDir = normalize(fragPos - pos);
	vec3 NORMAL = normalize(norm);

	if (dirLight.isOn == 1){
		
	    color = calcDirLight(dirLight, NORMAL, viewDir);
	}
	else if (pointLight.isOn == 1) {
	    color = calcPointLight(pointLight, NORMAL, fragPos, viewDir);
	}
	else {
        color = vec3(0.23f, 0.34f, 0.8f);
	}


	outColor = vec4(color, 1.0f);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.dir);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0f / (0.9f * distance + 0.32f * (distance * distance));    
 
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 