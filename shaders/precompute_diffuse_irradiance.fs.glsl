
#version 330 core

layout(location = 0) out vec4 out_color;

in vec3 vs_out_position;

#include "precompute_common.glsl"

#define NUM_DIFF_SAMPLES 16384U

uniform samplerCube u_environment_map;

vec3 PrefilterEnvMapDiffuse(vec3 N, vec3 pixel) {
	vec3 color = vec3(0);

	//[loop]
	for (uint i = 0U; i < NUM_DIFF_SAMPLES; ++i) {
		vec2 E = Hammersley(i, NUM_DIFF_SAMPLES);
		vec3 L = TangentToWorld(CosineSample(E), N);

		color += textureLod(u_environment_map, L, 0).rgb;
	}

	return color / NUM_DIFF_SAMPLES;
}

void main() {

    const vec3 view_position = vec3(0);
    vec3 v = normalize(vs_out_position - view_position);

    out_color.rgb = PrefilterEnvMapDiffuse(v, vs_out_position);
    out_color.a = 1;

}
