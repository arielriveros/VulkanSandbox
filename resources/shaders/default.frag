#version 450

layout(binding = 1) uniform sampler2D TextureSampler;

layout(location = 0) in vec3 FragColor;
layout(location = 1) in vec2 FragTexCoord;


layout(location = 0) out vec4 OutColor;

void main() {
    OutColor = vec4(FragColor, 1.0) * texture(TextureSampler, FragTexCoord);
}