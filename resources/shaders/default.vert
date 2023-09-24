#version 450

layout(push_constant) uniform PushConstant {
    mat4 model;
    mat4 normal;
} pc;

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 viewProjection;
} sceneUbo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 FragNormal;
layout(location = 1) out vec2 FragTexCoord;

void main() {
    gl_Position = sceneUbo.viewProjection * pc.model * vec4(inPosition, 1.0);
    FragNormal = (pc.normal * vec4(inNormal, 1.0)).xyz;
    FragTexCoord = inTexCoord;
}