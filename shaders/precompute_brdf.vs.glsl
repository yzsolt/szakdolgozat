
#version 330 core

layout(location = 0) in vec2 vs_in_position;

out gl_PerVertex { vec4 gl_Position; };

out vec2 vs_out_position;

void main() {
    vs_out_position = vs_in_position;
    gl_Position = vec4(vs_in_position, 0, 1);
}
