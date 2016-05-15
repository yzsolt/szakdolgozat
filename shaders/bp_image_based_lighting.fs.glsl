
#version 330 core

layout(location = 0) out vec4 out_color;

in vec3 vs_out_position;
in vec3 vs_out_world_position;
in vec3 vs_out_normal;
in vec3 vs_out_tangent;
in vec2 vs_out_texture;

in mat3 vs_out_tbn;

uniform vec3 u_view_position;

#include "bp_pb_common.glsl"

uniform BlinnPhongMaterial u_bpm;

uniform samplerCube u_environment_map;
uniform samplerCube u_diffuse_irradiance_map;
uniform samplerCube u_specular_irradiance_map;

const vec3 light_position = vec3(60, 20, 0);

void main() {

    vec3 normal;
    vec3 light_direction = normalize(light_position - vs_out_position);
    vec3 view_direction = normalize(u_view_position - vs_out_position);

    if (u_bpm.normal.use_texture) {

        vec3 normal_map_value = normalize(texture(u_bpm.normal.texture, vs_out_texture).rgb * 2 - 1);
        normal = normalize(vs_out_tbn * normal_map_value);

        //light_direction = vs_out_tbn * light_direction;
        //view_direction = vs_out_tbn * view_direction;

    } else {
        normal = normalize(vs_out_normal);
    }

    vec3 I = normalize(vs_out_world_position - u_view_position);
    vec3 R = reflect(I, normal);

    float lambertian = max(dot(-R, normal), 0);
    float specular = 0;

    if (lambertian > 0) {

        vec3 half_direction = normalize(light_direction + view_direction);
        float specular_angle = max(dot(half_direction, normal), 0);
        specular = pow(specular_angle, u_bpm.shininess);

    }

    vec4 ambient_color = u_bpm.ambient.use_texture ? texture(u_bpm.ambient.texture, vs_out_texture) : u_bpm.ambient.color;
    vec4 diffuse_color = u_bpm.diffuse.use_texture ? texture(u_bpm.diffuse.texture, vs_out_texture) : u_bpm.diffuse.color;

    float specularity = u_bpm.specular.use_texture ? texture(u_bpm.specular.texture, vs_out_texture).r : 1;
    vec4 specular_component = specular * specularity * u_bpm.specular.color;

    // Environment mapping
    //R.z *= -1;
    //R = vec3(1, 0, 0);
    float reflectivity = u_bpm.reflection.use_texture ? texture(u_bpm.reflection.texture, vs_out_texture).r : 0;
    vec4 reflect_color = vec4(0);
    if (reflectivity > 0.05) {
        reflect_color = texture(u_environment_map, R) * reflectivity;
    }
    reflect_color.a = 1;

    vec4 diffuse_irradiance = texture(u_diffuse_irradiance_map, normal);
    //vec4 specular_color = texture(u_specular_irradiance_map, R);

    vec4 blinn_phong_color = ambient_color + /*lambertian */ reflect_color + lambertian * diffuse_color + diffuse_irradiance;// + specular_component;

    out_color = vec4(blinn_phong_color.rgb, 1);

}
