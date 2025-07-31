
#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 Tangent;

void main() {
    FragPos = vec3(uModel * vec4(aPosition, 1.0));
    Normal = uNormalMatrix * aNormal;
    TexCoord = aTexCoord;
    Tangent = uNormalMatrix * aTangent;
    
    gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}
