#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;
uniform bool useBlinnPhong;
uniform sampler2D shadowMap;
uniform bool shadowsEnabled;

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

void main(){
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(norm, lightDir), 0.0);
    float phongSpecular = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    float blinnShininess = 4.0 * shininess;
    float blinnSpecular = pow(max(dot(norm, halfwayDir), 0.0), blinnShininess);
    
    float spec = useBlinnPhong ? blinnSpecular : phongSpecular;
    float shadow = getShadow(norm, lightDir);

    vec3 ambient = ambientStrength * lightColor;
    vec3 diffuse = diffuseStrength * diff * lightColor;
    vec3 specular = specularStrength * spec * lightColor;
    vec3 result = ambient * objectColor +
        (1.0 - shadow) * (diffuse * objectColor + specular);
    FragColor = vec4(result, 1.0);
}