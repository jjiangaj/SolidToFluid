#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 uv;

uniform vec3 viewPos;
uniform sampler2D floorTexture;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

void main()
{

    
    vec3 texdiffuse = texture(floorTexture, uv).xyz;
    vec3 texambient = texdiffuse;

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                light.quadratic * (distance * distance));

    // Ambient  
    vec3 ambient = light.ambient * texambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * texdiffuse * diff;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * material.specular * spec;

    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);

    //FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}