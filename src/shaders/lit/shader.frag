#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 uCamPos;

struct Material {
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float shininess;
    vec3 color;
};

struct DirLight { 
    vec3 color;
    vec3 direction;
};

struct PointLight {
    vec3 color;
    vec3 position;

    float constant;
    float linear;
    float quadratic;
};

uniform sampler2D uTexture;

uniform Material uMaterial;

#define NR_POINT_LIGHTS 6
#define NR_DIR_LIGHTS 6

uniform DirLight uDirLights[NR_DIR_LIGHTS];
uniform PointLight uPointLights[NR_POINT_LIGHTS];

out vec4 FragColor;

vec3 getPointLightOutput(PointLight pointLight, vec3 camDir) {
    // pre requrities
    vec3 lightDir = normalize(pointLight.position - FragPos);
    vec3 reflectedLight = reflect(-lightDir, Normal);
    float dist = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * (dist * dist));

    // 0 - 1 dots
    float diffuse = max(dot(lightDir, normalize(Normal)), 0.0);
    float specular = pow(max(dot(camDir, reflectedLight), 0.0), uMaterial.shininess);

    // Color or Texture?
    vec3 TexFrag = texture(uTexture, TexCoords).rgb;
    vec3 fragColor = uMaterial.color * TexFrag; 
    
    // phong colors
    vec3 ambientColor =           uMaterial.ambientStrength * fragColor;
    vec3 diffusedColor = diffuse * uMaterial.diffuseStrength * fragColor;
    vec3 specularColor = specular * uMaterial.specularStrength * fragColor;

    return ambientColor + (diffusedColor + specularColor) * pointLight.color * attenuation;
}

vec3 getDirLightOutput(DirLight dirLight, vec3 camDir) {
    // pre requrities
    vec3 lightDir = normalize(dirLight.direction);
    vec3 reflectedLight = reflect(-lightDir, Normal);

    // 0 - 1 dots
    float diffuse = max(dot(lightDir, normalize(Normal)), 0.0);
    float specular = pow(max(dot(camDir, reflectedLight), 0.0), uMaterial.shininess);

    // final output color of frag (nonlit)
    vec3 TexFrag = texture(uTexture, TexCoords).rgb;
    vec3 fragColor = uMaterial.color * TexFrag; 
    
    // phong colors
    vec3 ambientColor =           uMaterial.ambientStrength * fragColor;
    vec3 diffusedColor = diffuse * uMaterial.diffuseStrength * fragColor;
    vec3 specularColor = specular * uMaterial.specularStrength * fragColor;

    return ambientColor + (diffusedColor + specularColor) * dirLight.color;
}

void main() {
    vec3 Output = vec3(0.0, 0.0, 0.0);

    // pre calculation 
    vec3 camDir = normalize(uCamPos - FragPos);

    // for pointlight in pointlights:
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        Output += getPointLightOutput(uPointLights[i], camDir);
    }

    for (int i = 0; i < NR_DIR_LIGHTS; i++) {
        Output += getDirLightOutput(uDirLights[i], camDir);
    }

    FragColor = vec4(Output, 1.0);
}