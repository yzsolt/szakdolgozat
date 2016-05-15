
#version 330 core

#include "light_common.glsl"

out mat3 vs_out_tbn;

void main() {

	vec4 world_position = u_world * vec4(vs_in_position, 1);

	vs_out_texture = vs_in_texture;
	vs_out_view_direction = normalize(u_view_position - world_position.xyz);

	vs_out_world_normal = (vec4(vs_in_normal, 0) * u_world_inverse).xyz;

	vec3 T = normalize(vec3(u_world * vec4(vs_in_tangent, 0)));
	vec3 B = normalize(vec3(u_world * vec4(vs_in_bitangent, 0)));

	vs_out_tbn = mat3(T, B, vs_out_world_normal);

	gl_Position = u_view_projection * world_position;

}
