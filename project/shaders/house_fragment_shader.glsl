#version 430 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform bool hasDiffuseMap;
uniform bool hasNormalMap;
uniform bool hasRoughnessMap;
uniform vec3 materialDiffuseColor;
uniform vec3 materialSpecularColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float materialShininess;

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

void main() {
    vec3 baseColor = materialDiffuseColor;
    if (hasDiffuseMap) {
        baseColor *= texture(diffuseMap, TexCoord).rgb;
    }

    float roughness = hasRoughnessMap
        ? texture(roughnessMap, TexCoord).r
        : clamp(1.0 - materialShininess / 128.0, 0.05, 1.0);
    float specularPower = mix(128.0, 4.0, roughness);

    vec3 normal = getSurfaceNormal();
    vec3 lightDirection = normalize(lightPos - FragPos);
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    float specularFactor = pow(max(dot(normal, halfwayDirection), 0.0), specularPower);

    vec3 ambient = ambientStrength * lightColor * baseColor;
    vec3 diffuse = diffuseFactor * lightColor * baseColor;
    vec3 specular = specularFactor * lightColor * materialSpecularColor;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
