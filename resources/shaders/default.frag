#version 450

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 viewProjection;
    vec4 dirLightPos;   // xyz = position, w = intensity
} scene;

layout(set = 1, binding = 0) uniform MaterialUBO {
    vec4 color;
} material;

layout(set = 1, binding = 1) uniform sampler2D baseTexture;

layout(location = 0) in vec4 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(scene.dirLightPos - fragPos).xyz;

    // ambient
    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

    vec3 objectColor = (material.color * texture(baseTexture, fragUV)).xyz;

    outColor = vec4((ambient + diffuse) * objectColor, 1.0);
}