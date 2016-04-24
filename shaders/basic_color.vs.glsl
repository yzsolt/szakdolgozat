
#version 330 core

layout(location = 0) in vec3 vs_in_position;

out gl_PerVertex { vec4 gl_Position; };

uniform mat4 u_world;
uniform mat4 u_view_projection;

void main() {
    gl_Position = u_view_projection * (u_world * vec4(vs_in_position, 1));
}
