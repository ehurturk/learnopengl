<VERTEX>
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

layout(std140) uniform Matrices {
    mat4 projview;
};

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projview * model * vec4(aPos, 1.0);
}

<FRAGMENT>
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
}
fs_in;

struct Material {
	sampler2D texture_diffuse1;
};

uniform Material material;

uniform sampler2D shadow_map;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool soft_shadows;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadow_map, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

	float shadow = 0.0f;
	if (soft_shadows == true) {
		vec2 texel_size = 1.0f/ textureSize(shadow_map, 0);
		// total of 9 samples (3x3 kernel)
		for (int x = -1; x <= 1; ++x) {
			for (int y = -1; y <= 1; ++y) {
				float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texel_size).r;
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
			}
		}

		shadow /= 9.0;
	}
	else {
		shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	}

	// if the coordinates are out of frustrum, then simply they aren't in shadows...
	if (projCoords.z > 1.0) 
		shadow = 0.0;

    return shadow;
}

void main() {
    vec3 color      = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 normal     = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff    = max(dot(lightDir, normal), 0.0);
    vec3 diffuse  = diff * lightColor;
    // specular
    vec3 viewDir    = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec            = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular   = spec * lightColor;
    // calculate shadow
	
    float shadow  = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}