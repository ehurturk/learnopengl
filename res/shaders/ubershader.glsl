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

out vec3 Normal;
out vec2 TexCoords;

#define NR_LIGHTS 4
out vec3 TangentLightPos[NR_LIGHTS];
out vec3 TangentDirLightDir;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform vec3 light_pos[4];

uniform vec3 directional_light_dir;

uniform vec3 cam_pos;

uniform mat4 model;
uniform mat3 model_corrected;


void main() {
    gl_Position = projview * model * vec4(aPos, 1.0);
    Normal      = model_corrected * aNormal;
    TexCoords   = aTexCoords;
    
    vec3 T = normalize(model_corrected * aTangent);
    vec3 N = normalize(model_corrected * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));

    mat3 TBN = transpose(mat3(T, B, N));
    TangentViewPos = TBN * cam_pos;
    vec3 FragPos     = vec3(model * vec4(aPos, 1.0f));
    TangentFragPos = TBN * FragPos;
    for (int i = 0; i < NR_LIGHTS; i++) 
        TangentLightPos[i] = TBN * light_pos[i];
    TangentDirLightDir = TBN * directional_light_dir;

}

<FRAGMENT>
#version 330 core

in vec3 Normal;
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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
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

in vec3 TangentLightPos[NR_POINT_LIGHTS];
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 TangentDirLightDir;

uniform bool normal_enabled;

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 tangent_dir) {
    vec3 lightDir          = normalize(-tangent_dir);
    vec3 reflect_direction = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float amb  = 0.2f;

    vec3 ambient  = amb * light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 diffuse  = diff * light.diffuse * texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 specular = spec * light.specular * texture(material.texture_specular1, TexCoords).rgb;

    // calculate shadow

    return ambient + diffuse + specular;
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 tangent_pos) {
    float distance    = length(tangent_pos - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
    // float attenuation = 1.0f /  max((distance*distance), 0.00001);

    vec3 lightDir   = normalize(tangent_pos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float amb  = 1.0;

    vec3 ambient           = amb * light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;// ambiant strength (pixel value) * ambient color * diffuse map
    vec4 diffuse_color_tex = texture(material.texture_diffuse1, TexCoords);                          // diffuse strength (pixel value) * diffuse color * diffuse map
    if (diffuse_color_tex.a < 0.1)
        discard;
    vec3 diffuse  = diff * light.diffuse * diffuse_color_tex.rgb;
    vec3 specular = spec * light.specular * texture(material.texture_specular1, TexCoords).rgb;// specular strength (pixel value) * specular color * specular map

    return (ambient + diffuse + specular) * attenuation;
}

void main() {
    vec3 color = vec3(0.0f);
    vec3 norm = vec3(0.0f);
    vec3 viewDir = vec3(0.0f);

    if (normal_enabled != true) 
        norm    = normalize(Normal);
    else {
        norm = texture(material.texture_normal1, TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);
    }

    viewDir = normalize(TangentViewPos - TangentFragPos);

    color += calculate_directional_light(directional_light, norm, viewDir, TangentDirLightDir);

    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        color += calculate_point_light(point_lights[i], norm, TangentFragPos, viewDir, TangentLightPos[i]);
    }

    FragColor = vec4(color, 1.0);
}