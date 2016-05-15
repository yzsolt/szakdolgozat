
#version 330 core

layout(location = 0) in vec3 vs_in_position;
layout(location = 1) in vec3 vs_in_normal;
layout(location = 2) in vec3 vs_in_tangent;
layout(location = 3) in vec3 vs_in_bitangent;
layout(location = 4) in vec2 vs_in_texture;

out gl_PerVertex { vec4 gl_Position; };

out vec3 vs_out_position;
out vec3 vs_out_world_position;
out vec3 vs_out_normal;
out vec3 vs_out_tangent;
out vec2 vs_out_texture;

out mat3 vs_out_tbn;

uniform mat4 u_world;
uniform mat4 u_view_projection;

uniform mat4 u_world_view;
uniform mat4 u_normal_matrix;

void main() {

    vec4 world_position = u_world * vec4(vs_in_position, 1);

    // TBN calculation for normal mapping

    vs_out_normal = normalize(vec3(u_world * vec4(vs_in_normal, 0)));

    vec3 T = normalize(vec3(u_world * vec4(vs_in_tangent, 0)));
    vec3 B = normalize(vec3(u_world * vec4(vs_in_bitangent, 0)));

    //T = normalize(T - dot(T, vs_out_normal) * vs_out_normal);

    vs_out_tbn = mat3(T, B, vs_out_normal);

    //

    vs_out_tangent = vs_in_tangent;
    vs_out_texture = vs_in_texture;

    gl_Position = u_view_projection * world_position;

    vec4 vertex_position = u_world_view * vec4(vs_in_position, 1);
    vs_out_position = vec3(vertex_position) / vertex_position.w;

    vs_out_world_position = vec3(u_world * vec4(vs_in_position, 1));

}
