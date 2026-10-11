#version 450 core

out vec4 FragColor;
in vec2 TexCoords;

struct Material {
    vec3 diffuseColor;
    sampler2D uTexture;
};

uniform Material uMaterial;
uniform bool isTexture;

void main() {
    vec3 TexFrag = isTexture ? texture(uMaterial.uTexture, TexCoords).rgb : vec3(1.0);
    vec3 albedo = uMaterial.diffuseColor * TexFrag;
    FragColor = vec4(albedo, 1.0);
}