#version 450

layout(set = 1, binding = 0) uniform sampler2D TextureSampler;

layout(location = 0) in vec3 FragNormal;
layout(location = 1) in vec2 FragTexCoord;

layout(location = 0) out vec4 OutColor;

void main() {
    OutColor = normalize(vec4(FragNormal, 1.0)) * texture(TextureSampler, FragTexCoord);
}