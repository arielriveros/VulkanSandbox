#version 450

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 viewProjection;
    vec4 cameraPos;
    vec4 dirLightDir;   // xyz = direction, w = intensity
    vec4 dirLightColor; // rgb = color, a = ambient intensity
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

void main() {
    // Normalize the surface normal
    vec3 normal = normalize(fragNormal);

    // Calculate the direction to the light
    vec3 lightDir = normalize(scene.dirLightDir.xyz);

    // Calculate the diffuse term
    float diffuse = max(dot(normal, lightDir), 0.0);
    vec4 diffuseColor = scene.dirLightColor * diffuse * material.diffuse;

    // Calculate the specular term
    vec3 viewDir = normalize(vec3(scene.cameraPos - fragPos));
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.specular.a);
    vec4 specularColor = scene.dirLightColor * spec * material.specular;

    // Calculate the ambient term
    vec4 ambientColor = scene.dirLightColor.a * scene.dirLightColor * material.ambient;

    // Calculate the final color
    vec4 color = texture(baseTexture, fragUV);
    outColor = color * (ambientColor + diffuseColor + specularColor) * scene.dirLightDir.w ;
}