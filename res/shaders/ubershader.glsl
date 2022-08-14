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

out VS_OUT {
    vec3 Normal;
    vec2 TexCoords;
    vec3 FragPos;

    mat3 TBN;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 model;
uniform mat3 model_corrected;

uniform mat4 lightSpaceMatrix;

void main() {
    vs_out.Normal      = model_corrected * aNormal;
    vs_out.TexCoords   = aTexCoords;
    vs_out.FragPos     = vec3(model * vec4(aPos, 1.0f));
    
    // vec3 T = normalize(model_corrected * aTangent);
    // vec3 N = normalize(model_corrected * aNormal);
    // T = normalize(T - dot(T, N) * N);
    // vec3 B = normalize(cross(N, T));
    vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    vs_out.TBN = mat3(T, B, N);

    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    
    gl_Position = projview * model * vec4(aPos, 1.0);
}

<FRAGMENT>
#version 330 core

in VS_OUT {
    vec3 Normal;
    vec2 TexCoords;
    vec3 FragPos;

    mat3 TBN; // normal mapping
    vec4 FragPosLightSpace; // shadow mapping
} fs_in;

out vec4 FragColor;

// Definitions
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

// Uniforms
uniform Material material;
uniform DirectionalLight directional_light;

#define NR_POINT_LIGHTS 4
uniform PointLight point_lights[NR_POINT_LIGHTS];

uniform vec3 cam_pos;

uniform sampler2D shadow_map;

uniform bool normal_enabled;
uniform bool soft_shadows;

// Function declerations
vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float calculate_directional_shadows(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

vec3 calculate_directional_light(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir          = normalize(-light.direction);
    vec3 reflect_direction = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float amb  = 1.0f;

    vec3 ambient  = amb * light.ambient * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 diffuse  = diff * light.diffuse * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 specular = spec * light.specular * texture(material.texture_specular1, fs_in.TexCoords).rgb;

    float shadow = calculate_directional_shadows(fs_in.FragPosLightSpace, normal, lightDir);

    return ambient + (diffuse + specular) * (1.0 - shadow);
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
    // float attenuation = 1.0f /  max((distance*distance), 0.00001);

    vec3 lightDir   = normalize(light.position - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float amb  = 0.1;

    vec3 ambient           = amb * light.ambient * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;// ambiant strength (pixel value) * ambient color * diffuse map
    vec4 diffuse_color_tex = texture(material.texture_diffuse1, fs_in.TexCoords);                          // diffuse strength (pixel value) * diffuse color * diffuse map
    if (diffuse_color_tex.a < 0.1)
        discard;
    vec3 diffuse  = diff * light.diffuse * diffuse_color_tex.rgb;
    vec3 specular = spec * light.specular * texture(material.texture_specular1, fs_in.TexCoords).rgb;// specular strength (pixel value) * specular color * specular map

    return (ambient + diffuse + specular) * attenuation;
}

float calculate_directional_shadows(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 

    float closestDepth = texture(shadow_map, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    if (soft_shadows == true) {
        vec2 texel_size = 1.0/textureSize(shadow_map, 0);
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texel_size).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0: 0.0;
            }
        }
        shadow = shadow / 9.0;
    } else {
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }

    // if the coordinates are out of frustrum, then simply they aren't in shadows...
    // TODO: Solve this with cascading shadow maps
    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main() {
    vec3 color = vec3(0.0f);
    vec3 norm = vec3(0.0f);

    if (normal_enabled != true) 
        norm    = normalize(fs_in.Normal);
    else {
        norm = texture(material.texture_normal1, fs_in.TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);
        norm = normalize(fs_in.TBN * norm);
    }

    vec3 viewDir = normalize(cam_pos - fs_in.FragPos);
    color += calculate_directional_light(directional_light, norm, viewDir);

    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        color += calculate_point_light(point_lights[i], norm, fs_in.FragPos, viewDir);
    }

    FragColor = vec4(color, 1.0f);
}