#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uModel;
uniform mat4 uProjection;
uniform mat4 uView;

out vec2 TexCoords;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}
