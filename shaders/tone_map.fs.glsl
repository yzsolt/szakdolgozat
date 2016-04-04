
#version 330 core

layout(location = 0) out vec4 out_color;

uniform sampler2D u_hdr_texture;
uniform sampler2D u_average_luminance_texture;
uniform int u_tone_map;
uniform float u_avg;
in vec2 vs_out_texture;

#define REINHARD    0
#define UNCHARTED_2 1

// Uncharted 2

const float A = 0.22;
const float B = 0.30;
const float C = 0.10;
const float D = 0.20;
const float E = 0.01;
const float F = 0.30;
const float W = 11.2;

vec3 uncharted2_tone_map(vec3 x) {
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

void main() {

    vec3 hdr_color = texture(u_hdr_texture, vs_out_texture).rgb * u_avg;

    switch (u_tone_map) {

        case REINHARD:
            out_color = vec4(hdr_color / (hdr_color + 1), 1);
            break;

        case UNCHARTED_2:

        	float Lavg = texelFetch(u_average_luminance_texture, ivec2(0, 0), 0).r;
        	float two_ad_EV = Lavg * (100.0 / 12.5);
        	float exposure = 1.0 / (1.2 * two_ad_EV);

        	vec4 base = texture(u_hdr_texture, vs_out_texture) * 8;

        	vec3 lincolor = uncharted2_tone_map(base.rgb * exposure);
        	vec3 invlinwhite = 1.0 / uncharted2_tone_map(vec3(W));

        	base.rgb = lincolor * invlinwhite;

        	out_color = base;
            //out_color = vec4(Lavg, 0, 0, 1);

            break;

    }

}
