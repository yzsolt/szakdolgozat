
#version 330 core

layout(location = 0) in vec3 vs_in_position;

out gl_PerVertex { vec4 gl_Position; };

out vec3 vs_out_position;

uniform mat4 u_view_projection;

void main() {

    vs_out_position = vs_in_position;

    vec4 position = u_view_projection * vec4(vs_in_position, 1);
    gl_Position = position.xyww;

}
