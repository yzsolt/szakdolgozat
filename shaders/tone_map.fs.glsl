
#version 330 core

layout(location = 0) out vec4 out_color;

uniform sampler2D u_hdr_texture;
uniform sampler2D u_average_luminance_texture;
uniform float u_exposure;
uniform int u_tone_map;

in vec2 vs_out_texture;

#define REINHARD    0
#define UNCHARTED_2 1
#define UNREAL_4	2

vec3 unreal_4_tone_map(vec3 hdr_color) {
	vec3 gamma_color = hdr_color / (hdr_color + 0.187) * 1.035;
	return pow(gamma_color, vec3(2.2));
}

vec3 reinhard_tone_map(vec3 hdr_color) {
	return hdr_color / (hdr_color + 1);
}

const float A = 0.22;
const float B = 0.30;
const float C = 0.10;
const float D = 0.20;
const float E = 0.01;
const float F = 0.30;
const float W = 11.2;

vec3 uncharted_2_tone_map(vec3 hdr_color) {
	return ((hdr_color * (A * hdr_color + C * B) + D * E) / (hdr_color * (A * hdr_color + B) + D * F)) - E / F;
}

void main() {

    vec3 hdr_color = texture(u_hdr_texture, vs_out_texture).rgb;
	float exposure;

	if (u_exposure < 0) {
		float average_luminance = max(0.0001, texelFetch(u_average_luminance_texture, ivec2(0, 0), 0).r);
		exposure = 1 / (9.6 * average_luminance);
	} else {
		exposure = u_exposure;
	}

    switch (u_tone_map) {

        case REINHARD:
            out_color.rgb = reinhard_tone_map(hdr_color * exposure);
            break;

        case UNCHARTED_2:

        	vec3 linear_color = uncharted_2_tone_map(hdr_color * exposure);
        	vec3 inverse_linear_white = 1 / uncharted_2_tone_map(vec3(W));

        	out_color.rgb = linear_color * inverse_linear_white;

            break;

		case UNREAL_4:
			out_color.rgb = unreal_4_tone_map(hdr_color * exposure);
			break;

    }

	out_color.a = 1;

}
