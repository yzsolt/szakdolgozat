
#version 330 core

layout(location = 0) in vec3 vs_in_position;
layout(location = 1) in vec3 vs_in_normal;
layout(location = 2) in vec3 vs_in_tangent;
layout(location = 3) in vec3 vs_in_bitangent;
layout(location = 4) in vec2 vs_in_texture;

out gl_PerVertex { vec4 gl_Position; };

uniform mat4 u_world;
uniform mat4 u_world_inverse;
uniform mat4 u_view_projection;

uniform vec3 u_view_position;

out vec2 vs_out_texture;
out vec3 vs_out_world_normal;
out vec3 vs_out_view_direction;

void main() {

	vec4 world_position = u_world * vec4(vs_in_position, 1);

	vs_out_texture = vs_in_texture;
	vs_out_view_direction = u_view_position - world_position.xyz;
	vs_out_world_normal = (vec4(vs_in_normal, 0) * u_world_inverse).xyz;

	gl_Position = u_view_projection * world_position;

}
