
#version 330 core

layout(location = 0) out vec4 out_color;

uniform sampler2D u_previous_adapted_luminance_texture;
uniform sampler2D u_average_luminance_texture;
uniform float u_elapsed_time;

const float ADAPTATION_SPEED = 60.f;

void main() {

	float previous_adaptation = texelFetch(u_previous_adapted_luminance_texture, ivec2(0, 0), 0).r;
	float average_luminance = texelFetch(u_average_luminance_texture, ivec2(0, 0), 6).r;
	float adaptated_color = previous_adaptation + (average_luminance - previous_adaptation) * (1 - pow(0.98, ADAPTATION_SPEED * u_elapsed_time));

	out_color = vec4(adaptated_color, 0, 0, 1);

}
