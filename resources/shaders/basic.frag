#version 450

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 viewProjection;
    vec4 cameraPos;
    vec4 dirLightPos;   // lighting unused
    vec4 dirLightColor; // lighting unused
} scene;

layout(set = 1, binding = 0) uniform MaterialUBO {
    vec4 diffuse;
    vec4 specular; // unused
    vec4 ambient;  // unused
} material;

layout(set = 1, binding = 1) uniform sampler2D baseTexture;

layout(location = 0) in vec4 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = material.diffuse * texture(baseTexture, fragUV);
}