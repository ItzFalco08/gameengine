#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat3 normalMatrix;

// O U T S
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // O U T S (Normal and Pos of fragment)
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    Normal = normalMatrix * aNormal; // apply correct rotation, scaling to normal
}