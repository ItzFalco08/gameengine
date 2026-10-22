#version 450 core

#define MAX_LIGHTS_PER_CLUSTER 128
#define MAX_DIR_LIGHTS 6

// SSBOs
struct ClusterLights {
	uint lightsIndices[MAX_LIGHTS_PER_CLUSTER];
};
struct PointLight { 
	vec4 positionRange;
	vec4 colorIntensity;
};
layout(std430, binding = 0) readonly buffer LightSSBO { PointLight lights[]; };
layout(std430, binding = 2) readonly buffer CountSSBO { uint counts[]; };
layout(std430, binding = 3) readonly buffer IndexSSBO { ClusterLights indices[]; };

in vec3 FragPos;
in vec3 FragPosViewSpace;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

uniform vec3 uCamPos;

struct Material {
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
    sampler2D uDiffuseMap;
    sampler2D uSpecularMap;
    sampler2D uNormalMap;
};

struct DirLight { 
    vec3 color;
    vec3 direction;
};

uniform Material uMaterial;
uniform DirLight uDirLights[MAX_DIR_LIGHTS];

uniform int nDirLights;
uniform float zNear;
uniform float zFar;
uniform uvec3 gridSize;
uniform vec2 uTileSize;

out vec4 FragColor;

vec3 getPointLightOutput(PointLight pointLight, vec3 camDir, vec3 fragColor, vec3 N, vec3 specularTexel, vec3 diffTexel) {
    // pre requrities
    float dist = length(pointLight.positionRange.xyz - FragPos);
     
    vec3 lightDir = normalize(pointLight.positionRange.xyz - FragPos);
    vec3 reflectedLight = reflect(-lightDir, N);
    float attenuation = pow(clamp(1.0 - (dist / pointLight.positionRange.w), 0.0, 1.0), 2.0) * pointLight.colorIntensity.w;

    // 0 - 1 dots
    float diffuse = max(dot(lightDir, normalize(N)), 0.0);
    float specular = pow(max(dot(camDir, reflectedLight), 0.0), uMaterial.shininess);

    // phong colors
    vec3 diffusedColor = diffuse * uMaterial.diffuseColor * diffTexel * fragColor;
    vec3 specularColor = specular * uMaterial.specularColor * specularTexel * fragColor;

    return (diffusedColor + specularColor) * pointLight.colorIntensity.xyz * attenuation;
}

vec3 getDirLightOutput(DirLight dirLight, vec3 camDir, vec3 fragColor, vec3 N, vec3 specularTexel, vec3 diffTexel) {
    // pre requrities
    vec3 lightDir = normalize(dirLight.direction);
    vec3 reflectedLight = reflect(-lightDir, N);

    // 0 - 1 dots
    float diffuse = max(dot(lightDir, N), 0.0);
    float specular = pow(max(dot(camDir, reflectedLight), 0.0), uMaterial.shininess);

    // phong colors
    vec3 diffusedColor = diffuse * uMaterial.diffuseColor * diffTexel * fragColor;
    vec3 specularColor = specular * uMaterial.specularColor * specularTexel * fragColor;

    return  (diffusedColor + specularColor) * dirLight.color;
}

void main() {
    vec3 Output = vec3(0.0, 0.0, 0.0);

    vec3 diffTexel = texture(uMaterial.uDiffuseMap, TexCoords).rgb; // guarenteed texture
    vec3 specularTexel = texture(uMaterial.uSpecularMap, TexCoords).rgb; // guarenteed texture
    vec3 normalTexel = texture(uMaterial.uNormalMap, TexCoords).rgb; // guarenteed texture

    vec3 ambient = uMaterial.diffuseColor * diffTexel * 0.3f;

    // ambient
    Output += ambient;

    // Locating which cluster this fragment is part of
    uint zTile = min(uint((log(abs(FragPosViewSpace.z) / zNear) * gridSize.z) / log(zFar / zNear)), gridSize.z - 1u);
    vec2 tileSize = uTileSize;
    uvec3 tile = uvec3(gl_FragCoord.xy / tileSize, zTile);
    uint tileIndex = tile.x + (tile.y * gridSize.x) + (tile.z * gridSize.x * gridSize.y);
    
    // adds diffuse/specular from point and dir lights
    vec3 camDir = normalize(uCamPos - FragPos);
    vec3 N = normalize(TBN * (normalTexel.rgb * 2.0 - 1.0));              
    // tangent space -> world space

    uint count = min(counts[tileIndex], uint(MAX_LIGHTS_PER_CLUSTER));
    for (int i = 0; i < count; i++) {
        Output += getPointLightOutput(lights[indices[tileIndex].lightsIndices[i]], camDir, uMaterial.diffuseColor, N, specularTexel, diffTexel);
    }
    for (int i = 0; i < nDirLights; i++) {
        Output += getDirLightOutput(uDirLights[i], camDir, uMaterial.diffuseColor, N, specularTexel, diffTexel);
    }

    FragColor = vec4(Output, 1.0);
}
