
#version 330 core

layout(location = 0) out vec4 out_color;

in vec3 vs_out_position;

#include "precompute_common.glsl"

#define NUM_SPEC_SAMPLES 2048U

uniform samplerCube u_environment_map;
/*
uniform mat4 u_world;
uniform mat4 u_projection;
uniform mat4 u_cube_views[6];

uniform vec3 u_view_position;*/
uniform float u_roughness = 0.1f;

vec3 PrefilterEnvMapSpecular(float roughness, vec3 R) {
	vec3 color = vec3(0);
	float total_weight = 0;

	for (uint i = 0U; i < NUM_SPEC_SAMPLES; ++i) {
		vec2 E = Hammersley(i, NUM_SPEC_SAMPLES);
		vec3 H = TangentToWorld(GGXSample(E, roughness), R);
		vec3 L = 2 * dot(R, H) * H - R;

		float ndotl = saturate(dot(R, L));

		if (ndotl > 0) {
			color += textureLod(u_environment_map, L, 0).rgb * ndotl;
			total_weight += ndotl;
		}
	}

	return color / max(total_weight, 0.001f);
}

void main() {

    const vec3 view_position = vec3(0);
    vec3 v = normalize(vs_out_position - view_position);

    out_color.rgb = PrefilterEnvMapSpecular(u_roughness, v);
    out_color.a = 1;

}
