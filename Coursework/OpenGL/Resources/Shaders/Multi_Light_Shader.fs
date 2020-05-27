#version 330 core


struct PointLight {    
    vec4 position;
    
    float constant;
    float linear;
    float quadratic;  
	float PADDING;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};  

#define NR_POINT_LIGHTS 2


layout(std140) uniform  DirLightBlock
{
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
}dirLight;


layout(std140) uniform PointLightBlock
{
	PointLight pointLights[NR_POINT_LIGHTS];
};

// texture sampler
uniform sampler2D texture_diffuse1;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 viewPos;

vec3 CalcDirLight(vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

out vec4 FragColour;

void main()
{	
	// properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(norm, viewDir);
    
	// phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);      
    
    FragColour = vec4(result, 1.0);
}

vec3 CalcDirLight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirLight.direction.xyz);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);		//Shininess
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    // combine results
    vec3 ambient  = dirLight.ambient.rgb  * vec3(texture(texture_diffuse1, TexCoord));
    vec3 diffuse  = dirLight.diffuse.rgb  * diff * vec3(texture(texture_diffuse1, TexCoord));
    vec3 specular = 0.1 * dirLight.specular.rgb * spec;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);		//Shininess
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    // attenuation
    float distance    = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient.rgb  * vec3(texture(texture_diffuse1, TexCoord));
    vec3 diffuse  = light.diffuse.rgb  * diff * vec3(texture(texture_diffuse1, TexCoord));
    vec3 specular = 0.1 * light.specular.rgb * spec;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 