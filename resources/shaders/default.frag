#version 450

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
    vec4 properties; // x = constant, y = linear, z = quadratic, w = unused
};

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 viewProjection;
    vec4 cameraPos;

    DirectionalLight dirLight;
    PointLight pointLight;
} u_scene;

struct Material {
    vec4 diffuse;
    vec4 specular; // rgb = color, a = shininess
    vec4 ambient;
};

layout(set = 1, binding = 0) uniform MaterialUBO {
    Material material;
} u_material;

layout(set = 1, binding = 1) uniform sampler2D baseTexture;

layout(location = 0) in vec4 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

vec4 calcDirectionalLighting(vec3 normal, vec3 viewDir, DirectionalLight light, Material material) {
    // Calculate the direction to the light
    vec3 lightDir = normalize(light.direction.xyz);
    // Calculate the diffuse component
    float diffuseFactor = max(dot(normal, lightDir), 0.0f);
    vec4 diffuseColor = diffuseFactor * light.diffuse * material.diffuse;

    // Calculate the specular component blinn-phong
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0f), material.specular.a);
    vec4 specularColor = specularFactor * light.specular * material.specular;

    // Calculate the ambient component
    vec4 ambientColor = light.ambient * material.ambient;

    return (diffuseColor + specularColor + ambientColor);
}

vec4 calcPointLighting(vec3 normal, vec3 viewDir, PointLight light, Material material) {
    // Calculate the direction to the light
    vec3 lightDir = normalize(light.position.xyz - fragPos.xyz);

    // Calculate the diffuse component
    float diffuseFactor = max(dot(normal, lightDir), 0.0f);
    vec4 diffuseColor = diffuseFactor * light.diffuse * material.diffuse;

    // Calculate the specular component blinn-phong
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0f), material.specular.a);
    vec4 specularColor = specularFactor * light.specular * material.specular;

    // Calculate the ambient component
    vec4 ambientColor = light.ambient * material.ambient;

    // Calculate the attenuation
    float distance = length(light.position.xyz - fragPos.xyz);
    float attenuation = 1.0f / (light.properties.x + light.properties.y * distance + light.properties.z * (distance * distance));

    return (diffuseColor + specularColor + ambientColor) * attenuation;
}

void main() {
    // Normalize the surface normal
    vec3 normal = normalize(fragNormal);
    // Calculate the direction to the camera
    vec3 viewDir = normalize(u_scene.cameraPos.xyz - fragPos.xyz);

    vec4 color = vec4(0.0f);
    // Calculate the lighting
    color += calcDirectionalLighting(normal, viewDir, u_scene.dirLight, u_material.material);
    color += calcPointLighting(normal, viewDir, u_scene.pointLight, u_material.material);

    // Sample the texture
    vec4 texel = texture(baseTexture, fragUV);

    outColor = color * texel ;
}