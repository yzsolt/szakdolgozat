
layout(location = 0) in vec3 vs_in_position;
layout(location = 1) in vec3 vs_in_normal;
layout(location = 2) in vec3 vs_in_tangent;
layout(location = 3) in vec3 vs_in_bitangent;
layout(location = 4) in vec2 vs_in_texture;

uniform mat4 u_world;
uniform mat4 u_world_inverse;
uniform mat4 u_view_projection;

uniform vec3 u_view_position;

out gl_PerVertex { vec4 gl_Position; };

out vec2 vs_out_texture;
out vec3 vs_out_world_normal;
out vec3 vs_out_view_direction;
