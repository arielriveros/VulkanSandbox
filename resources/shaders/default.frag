#version 450

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 viewProjection;
    vec4 cameraPos;

    // Directional light
    vec4 lightDir;   // xyz = direction, w = unused
    vec4 lightDiffuse; // rgb = color, a = unused
    vec4 lightSpecular; // rgb = color, a = unused
    vec4 lightAmbient; // rgb = color, a = unused

} scene;

layout(set = 1, binding = 0) uniform MaterialUBO {
    vec4 diffuse;
    vec4 specular; // rgb = color, a = shininess
    vec4 ambient;
} material;

layout(set = 1, binding = 1) uniform sampler2D baseTexture;

layout(location = 0) in vec4 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

vec4 calcDirectionalLighting(
    vec3 normal, 
    vec3 viewDir, 
    vec3 lightDir, 
    vec4 Ld, 
    vec4 Ls, 
    vec4 La,
    vec4 Kd,
    vec4 Ks,
    vec4 Ka,
    float shininess
    ) {
    // Calculate the diffuse component
    float diffuseFactor = max(dot(normal, lightDir), 0.0f);
    vec4 diffuseColor = Ld * diffuseFactor * Kd;

    // Calculate the specular component blinn-phong
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0f), shininess);
    vec4 specularColor = Ls * specularFactor * Ks;

    // Calculate the ambient component
    vec4 ambientColor = La * Ka;

    return (diffuseColor + specularColor + ambientColor);
}

void main() {
    // Normalize the surface normal
    vec3 normal = normalize(fragNormal);
    // Calculate the direction to the light
    vec3 lightDir = normalize(scene.lightDir.xyz);
    // Calculate the direction to the camera
    vec3 viewDir = normalize(scene.cameraPos.xyz - fragPos.xyz);

    vec4 color = vec4(0.0f);
    // Calculate the lighting
    color += calcDirectionalLighting(
        normal, viewDir, lightDir,
        scene.lightDiffuse, scene.lightSpecular, scene.lightAmbient,
        material.diffuse, material.specular, material.ambient, material.specular.a);

    // Sample the texture
    vec4 texel = texture(baseTexture, fragUV);

    outColor = color * texel ;
}