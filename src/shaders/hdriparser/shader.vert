#version 450 core
layout (location = 0) in vec2 pos;
out vec2 FragPos;

void main() {
    gl_Position = vec4(pos, 0.0, 1.0);
    FragPos = pos;
}