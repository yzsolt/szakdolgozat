
#version 330 core

layout(location = 0) out vec4 out_color;

in vec3 vs_out_world_position;
in vec3 vs_out_world_normal;
in vec3 vs_out_view_direction;
in vec2 vs_out_texture;

in mat3 vs_out_tbn;

#include "bp_pb_common.glsl"

uniform BlinnPhongMaterial u_bpm;

uniform samplerCube u_environment_map;
uniform samplerCube u_diffuse_irradiance_map;
uniform samplerCube u_specular_irradiance_map;

void main() {

    vec3 normal;

    if (u_bpm.normal.use_texture) {

        vec3 normal_map_value = normalize(texture(u_bpm.normal.texture, vs_out_texture).rgb * 2 - 1);
        normal = normalize(vs_out_tbn * normal_map_value);

    } else {
        normal = normalize(vs_out_world_normal);
    }

    vec3 R = reflect(vs_out_view_direction, normal);
    vec3 light_direction = R;//normalize(light_position - vs_out_world_position);

    float lambertian = max(dot(-R, normal), 0);
    float specular_intensity = 0;

    if (lambertian > 0) {

        vec3 half_direction = normalize(light_direction + vs_out_view_direction);
        float specular_angle = max(dot(half_direction, normal), 0);
        specular_intensity = pow(specular_angle, u_bpm.shininess);

    }

    vec4 ambient_color = u_bpm.ambient.use_texture ? texture(u_bpm.ambient.texture, vs_out_texture) : u_bpm.ambient.color;
    vec4 diffuse_color = u_bpm.diffuse.use_texture ? texture(u_bpm.diffuse.texture, vs_out_texture) : u_bpm.diffuse.color;
    vec4 specular_color = u_bpm.specular.use_texture ? texture(u_bpm.specular.texture, vs_out_texture) : u_bpm.specular.color;

    float specularity = (specular_color.r + specular_color.g + specular_color.b) / 3;
    vec4 specular_component = specular_intensity * specularity * specular_color;


    float reflectivity = specularity;
    vec4 reflect_color = vec4(0);

    R.y *= -1;

    if (reflectivity > 0.05) {
        reflect_color = texture(u_environment_map, R) * reflectivity;
    }

    reflect_color.a = 1;

    vec4 diffuse_irradiance = texture(u_diffuse_irradiance_map, normal);

    vec4 blinn_phong_color = ambient_color +
                             lambertian * reflect_color +
                             lambertian * diffuse_color * diffuse_irradiance +
                             specular_component;

    out_color = vec4(blinn_phong_color.rgb, 1);

}
