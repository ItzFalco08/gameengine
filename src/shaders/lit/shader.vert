#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;

uniform mat4 uModel;
uniform mat4 uProjection;
uniform mat4 uView;
uniform mat3 uNormalMatrix;

// O U T S
out vec3 FragPos;
out vec3 FragPosViewSpace;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

void main() {
    vec3 N = normalize(uNormalMatrix * aNormal);
    vec3 T = normalize(mat3(uModel) * aTangent.xyz);
    T = normalize(T - dot(T, N) * N);              // re-orthogonalize against N
    vec3 B = cross(N, T) * aTangent.w;             // handedness from tangent.w

    TBN = mat3(T, B, N);

    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    
    // O U T S (Normal and Pos of fragment)
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    FragPosViewSpace = vec3(uView * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    Normal = normalize(uNormalMatrix * aNormal); // apply correct rotation, scaling to normal
}