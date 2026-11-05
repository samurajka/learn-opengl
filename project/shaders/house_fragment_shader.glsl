#version 430 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform bool hasDiffuseMap;
uniform bool hasNormalMap;
uniform bool hasRoughnessMap;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform bool shadowsEnabled;

layout(std430, binding = 0) readonly buffer MaterialBuffer {
    vec4 diffuseColor;
    vec4 specularColor;
    vec4 parameters;
} material;

vec3 getSurfaceNormal() {
    vec3 geometricNormal = normalize(Normal);
    if (!hasNormalMap) {
        return geometricNormal;
    }

    vec3 positionDx = dFdx(FragPos);
    vec3 positionDy = dFdy(FragPos);
    vec2 texCoordDx = dFdx(TexCoord);
    vec2 texCoordDy = dFdy(TexCoord);
    float determinant = texCoordDx.x * texCoordDy.y - texCoordDx.y * texCoordDy.x;
    if (abs(determinant) < 0.000001) {
        return geometricNormal;
    }

    vec3 tangent = normalize((positionDx * texCoordDy.y - positionDy * texCoordDx.y) / determinant);
    vec3 bitangent = normalize((-positionDx * texCoordDy.x + positionDy * texCoordDx.x) / determinant);
    tangent = normalize(tangent - geometricNormal * dot(geometricNormal, tangent));
    bitangent = normalize(cross(geometricNormal, tangent)) * sign(determinant);

    vec3 tangentNormal = texture(normalMap, TexCoord).rgb * 2.0 - 1.0;
    return normalize(mat3(tangent, bitangent, geometricNormal) * tangentNormal);
}

float getShadow(vec3 normal, vec3 lightDirection) {
    if (!shadowsEnabled) {
        return 0.0;
    }

    vec3 projected = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projected = projected * 0.5 + 0.5;
    if (projected.z > 1.0 || any(lessThan(projected.xy, vec2(0.0))) ||
        any(greaterThan(projected.xy, vec2(1.0)))) {
        return 0.0;
    }

    float bias = max(0.0005 * (1.0 - dot(normal, lightDirection)), 0.0001);
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float closestDepth = texture(
                shadowMap, projected.xy + vec2(x, y) * texelSize).r;
            shadow += projected.z - bias > closestDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main() {
    vec3 baseColor = material.diffuseColor.rgb;
    if (hasDiffuseMap) {
        baseColor *= texture(diffuseMap, TexCoord).rgb;
    }

    float roughness = hasRoughnessMap
        ? texture(roughnessMap, TexCoord).r
        : clamp(1.0 - material.parameters.y / 128.0, 0.05, 1.0);
    float specularPower = mix(128.0, 4.0, roughness);

    vec3 normal = getSurfaceNormal();
    vec3 lightDirection = normalize(lightPos - FragPos);
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    float specularFactor = pow(max(dot(normal, halfwayDirection), 0.0), specularPower);
    float shadow = getShadow(normal, lightDirection);

    vec3 ambient = material.parameters.x * lightColor * baseColor;
    vec3 diffuse = diffuseFactor * lightColor * baseColor;
    vec3 specular = specularFactor * lightColor * material.specularColor.rgb;
    FragColor = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0);
}
