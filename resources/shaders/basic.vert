#version 450

layout(push_constant) uniform PushConstant {
    mat4 transform;
    mat4 normal;
} model;

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 viewProjection;
    vec4 cameraPos;
    vec4 dirLightPos;   // lighting unused
    vec4 dirLightColor; // lighting unused
} scene;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;

void main() {
    fragPos = model.transform * vec4(inPosition, 1.0);
    fragUV = inUV;

    gl_Position = scene.viewProjection * fragPos;
}