
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

uniform int u_visualize;

uniform samplerCube u_environment_map;

const vec3 light_position = vec3(60, 20, 0);

void main() {

    vec3 normal;
    vec3 light_direction = normalize(light_position - vs_out_position);
    vec3 view_direction = normalize(u_view_position - vs_out_position);

    if (u_bpm.normal.use_texture) {

        vec3 normal_map_value = normal = normalize(texture(u_bpm.normal.texture, vs_out_texture).rgb * 2 - 1);
        normal = normalize(vs_out_tbn * normal_map_value);

        //light_direction = vs_out_tbn * light_direction;
        //view_direction = vs_out_tbn * view_direction;

    } else {
        normal = normalize(vs_out_normal);
    }

    float lambertian = max(dot(light_direction, normal), 0);
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

    vec3 I = normalize(vs_out_position - u_view_position);
    vec3 R = reflect(I, normalize(vs_out_normal));
    //R.z *= -1;
    float reflectivity = u_bpm.reflection.use_texture ? texture(u_bpm.reflection.texture, vs_out_texture).r : 0;
    vec4 reflect_color = vec4(0);
    if (reflectivity > 0.05) {
        reflect_color = texture(u_environment_map, R) * reflectivity;
    }

    vec4 blinn_phong_color = ambient_color + lambertian * reflect_color +
                             lambertian * diffuse_color +
                             specular_component;

    // Visualization for debugging

    switch (u_visualize) {

        case VISUALIZE_NOTHING:
            out_color = vec4(blinn_phong_color.rgb, 1.0);
            break;

        case VISUALIZE_POSITIONS:
            out_color = vec4(normalize(vs_out_position), 1.0);
            break;

        case VISUALIZE_NORMALS:
            out_color = vec4(normal / 2.0 + 0.5, 1.0);
            break;

        case VISUALIZE_TANGENTS:
            out_color = vec4(normalize(vs_out_tangent) / 2.0 + 0.5, 1.0);
            break;

        case VISUALIZE_AMBIENT:
            out_color = vec4(ambient_color.rgb, 1.0);
            break;

        case VISUALIZE_DIFFUSE:
            out_color = vec4(diffuse_color.rgb, 1.0);
            break;

        case VISUALIZE_NORMAL_MAP:
            out_color = u_bpm.normal.use_texture ? vec4(texture(u_bpm.normal.texture, vs_out_texture).rgb, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
            break;

        case VISUALIZE_SPECULARITY:
            out_color = vec4(vec3(specularity), 1.0);
            break;

        case VISUALIZE_REFLECTIVITY:
            out_color = vec4(vec3(reflectivity), 1.0);
            break;

    }

}
