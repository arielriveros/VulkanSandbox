#version 450

layout(set = 1, binding = 0) uniform MaterialUBO {
    vec4 color;
} materialUbo;
layout(set = 1, binding = 1) uniform sampler2D TextureSampler;

layout(location = 0) in vec3 FragNormal;
layout(location = 1) in vec2 FragTexCoord;

layout(location = 0) out vec4 OutColor;

void main() {
    OutColor = materialUbo.color * texture(TextureSampler, FragTexCoord);
}