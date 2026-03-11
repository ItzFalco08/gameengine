#version 330 core

out vec4 FragColor;

struct Material {
    vec3 color;
};

uniform Material uMaterial;

void main() {
    FragColor = vec4(uMaterial.color, 1.0);
}