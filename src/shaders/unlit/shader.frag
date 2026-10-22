#version 450 core

out vec4 FragColor;
in vec2 TexCoords;

struct Material {
    vec3 diffuseColor;
    sampler2D diffuseMap;
};

uniform Material uMaterial;

void main() {
    vec3 TexFrag = texture(uMaterial.diffuseMap, TexCoords).rgb;
    vec3 albedo = uMaterial.diffuseColor * TexFrag;
    FragColor = vec4(albedo, 1.0);
}
