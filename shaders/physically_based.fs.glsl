
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

uniform PhysicallyBasedMaterial u_pbm;

uniform int u_visualize;

uniform samplerCube u_skybox_cube_map;

const vec3 light_position = vec3(60, 20, 0);

void main() {

    vec4 diffuse_color;

    if (u_pbm.diffuse.use_texture) {
        diffuse_color = texture(u_pbm.diffuse.texture, vs_out_texture);
    } else {
        diffuse_color = u_pbm.diffuse.color;
    }

    vec3 normal;
    vec3 light_direction = normalize(light_position - vs_out_position);
    vec3 view_direction = normalize(u_view_position - vs_out_position);

    if (u_pbm.normal.use_texture) {

        vec3 normal_map_value = normal = normalize(texture(u_pbm.normal.texture, vs_out_texture).rgb * 2 - 1);
        normal = normalize(vs_out_tbn * normal_map_value);

        //light_direction = vs_out_tbn * light_direction;
        //view_direction = vs_out_tbn * view_direction;

    } else {
        normal = normalize(vs_out_normal);
    }

    //

    vec4 physically_based_color = vec4(1, 1, 0, 1);

    // Visualization for debugging

    switch (u_visualize) {

        case VISUALIZE_NOTHING:
            out_color = vec4(physically_based_color.rgb, 1.0);
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
            out_color = vec4(0, 0, 0, 1);
            break;

        case VISUALIZE_DIFFUSE:
            out_color = vec4(diffuse_color.rgb, 1.0);
            break;

        case VISUALIZE_NORMAL_MAP:
            out_color = u_pbm.normal.use_texture ? vec4(texture(u_pbm.normal.texture, vs_out_texture).rgb, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
            break;

        case VISUALIZE_SPECULARITY:
            out_color = vec4(0, 0, 0, 1);
            break;

        case VISUALIZE_REFLECTIVITY:
            //out_color = vec4(vec3(reflectivity), 1.0);
            out_color = vec4(0, 0, 0, 1);
            break;

    }

}
