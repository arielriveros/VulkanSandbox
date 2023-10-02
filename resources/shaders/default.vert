#version 450

layout(push_constant) uniform PushConstant {
    mat4 transform;
    mat4 normal;
} model;

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 viewProjection;
    vec4 cameraPos;
    vec4 dirLightDir;   // xyz = direction, w = intensity
    vec4 dirLightColor; // rgb = color, a = ambient intensity
} scene;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;

void main() {
    fragPos = model.transform * vec4(inPosition, 1.0);
    fragNormal = mat3(model.normal) * inNormal;
    fragUV = inUV;

    gl_Position = scene.viewProjection * fragPos;
}