#version 450

layout(push_constant) uniform PushConstant {
    mat4 transform;
    mat4 normal;
} u_model;

struct DirectionalLight {
    vec4 direction;
    vec4 diffuse;
    vec4 specular;
    vec4 ambient;
};

struct PointLight {
    vec4 position;
    vec4 diffuse;
    vec4 specular;
    vec4 ambient;
    float constant;
    float linear;
    float quadratic;
    float padding;
};

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 viewProjection;
    vec4 cameraPos;

    DirectionalLight dirLight;
    PointLight pointLight;
} u_scene;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;

void main() {
    fragPos = u_model.transform * vec4(inPosition, 1.0);
    fragNormal = mat3(u_model.normal) * inNormal;
    fragUV = inUV;

    gl_Position = u_scene.viewProjection * fragPos;
}