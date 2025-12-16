#version 330 core

out vec4 FragColor;

struct Material {
    vec3 ambient;
};

uniform Material material;

void main() {
    FragColor = vec4(material.ambient, 1.0);
}