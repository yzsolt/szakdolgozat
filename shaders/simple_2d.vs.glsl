
#version 330 core

layout(location = 0) in vec3 vs_in_position;
layout(location = 1) in vec2 vs_in_texture;

out gl_PerVertex { vec4 gl_Position; };

out vec3 vs_out_position;
out vec2 vs_out_texture;

void main() {
    vs_out_texture = vs_in_texture;
    gl_Position = vec4(vs_in_position, 1);
}
