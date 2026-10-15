#version 330 core
#define MAX_LIGHTS 5
in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coord;
out vec4 frag_color;

// rewrite later

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
};

uniform sampler2D tex_unit_id;

uniform Material material;

uniform vec3 view_pos;

vec3 calc_point_light(PointLight light, vec3 view_dir, vec3 norm) {
    vec3 light_direction = normalize(light.position - frag_pos);
    vec3 ambient = light.ambient * material.ambient;

    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    vec3 reflect_dir = reflect(-light_direction, norm);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main() {
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 norm = normalize(normal);
    vec3 result = vec3(0);
    for (uint i = 0u; i < point_light_count; i++) {
        result += calc_point_light(point_lights[i], view_dir, norm);
    }

    for (uint i = 0u; i < spot_light_count; i++) {
        result += calc_spot_light(spot_lights[i], view_dir, norm);
    }

    for (uint i = 0u; i < dir_light_count; i++) {
        result += calc_direction_light(directional_lights[i], view_dir, norm);
    }

    if (is_textured) {
        frag_color = vec4(result, 1.0) * texture(tex_unit_id, tex_coord);
    }
    else if (is_white) {
        result *= vec3(1);
        frag_color = vec4(result, 1.0);
    } else {
        result *= normal;
        frag_color = vec4(result, 1.0);
    }
}