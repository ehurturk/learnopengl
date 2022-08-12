<VERTEX>
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(std140) uniform Matrices {
    mat4 projview;
};

uniform mat4 model;
uniform mat3 model_corrected;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main() {
    gl_Position = projview * model * vec4(aPos, 1.0);
    FragPos     = vec3(model * vec4(aPos, 1.0f));
    Normal      = model_corrected * aNormal;
    TexCoords   = aTexCoords;
}

<FRAGMENT>
#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_emission1;
    sampler2D texture_normal1;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform DirectionalLight directional_light;

#define NR_POINT_LIGHTS 4
uniform PointLight point_lights[NR_POINT_LIGHTS];

uniform vec3 cam_pos;

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir          = normalize(-light.direction);
    vec3 reflect_direction = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    float am = 0.5f;
    vec3 ambient  = am * light.ambient * pow(vec4(1.0f), vec4(2.2)).rgb;
    vec3 diffuse  = diff * light.diffuse * pow(vec4(1.0f), vec4(2.2)).rgb;
    vec3 specular = spec * light.specular * pow(vec4(1.0f), vec4(2.2)).rgb;

    return ambient + diffuse + specular;
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0f / max((distance*distance), 0.00001);

    vec3 lightDir   = normalize(light.position - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float amb  = 1.0;

    vec3 ambient           = amb * light.ambient * pow(vec4(1.0f), vec4(2.2)).rgb;// ambient strength (pixel value) * ambient color * diffuse map
    vec4 diffuse_color_tex = pow(vec4(1.0f), vec4(2.2));                          // diffuse strength (pixel value) * diffuse color * diffuse map

    vec3 diffuse  = diff * light.diffuse * diffuse_color_tex.rgb;
    vec3 specular = spec * light.specular * pow(vec4(1.0f), vec4(2.2)).rgb;// specular strength (pixel value) * specular color * specular map

    return (ambient + diffuse + specular) * attenuation;
}

void main() {
    vec3 color = vec3(0.0f);
    vec3 norm = normalize(Normal);
    
    vec3 viewDir = normalize(cam_pos - FragPos);

    color += calculate_directional_light(directional_light, norm, viewDir);

    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        color += calculate_point_light(point_lights[i], norm, FragPos, viewDir);

    FragColor = vec4(color, 1.0);
}