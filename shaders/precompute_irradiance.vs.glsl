
#version 330 core

layout(location = 0) in vec2 vs_in_position;

out gl_PerVertex { vec4 gl_Position; };

uniform mat3 u_transformation;

out vec3 vs_out_position;

void main() {
    vs_out_position = u_transformation * vec3(vs_in_position, 1);
    gl_Position = vec4(vs_in_position, 0, 1);
}
