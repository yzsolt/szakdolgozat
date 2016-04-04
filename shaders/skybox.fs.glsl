
#version 330 core

layout(location = 0) out vec4 out_color;

in vec3 vs_out_position;

uniform samplerCube u_environment_map;

void main() {
    out_color = texture(u_environment_map, vs_out_position);
}
