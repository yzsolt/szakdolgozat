
#version 330 core

layout(location = 0) out vec2 out_color;

in vec2 vs_out_position;

#include "precompute_common.glsl"

#define NUM_SAMPLES	2048U

float Vis_Smith(float Roughness, float NoV, float NoL) {

	float a = Roughness * Roughness;
	float a2 = a * a;

	float Vis_SmithV = NoV + sqrt(NoV * (NoV - NoV * a2) + a2);
	float Vis_SmithL = NoL + sqrt(NoL * (NoL - NoL * a2) + a2);

	return 1 / (Vis_SmithV * Vis_SmithL);

}

float Vis_SmithJointApprox(float Roughness, float NoV, float NoL) {

	float a = Roughness * Roughness;

	float Vis_SmithV = NoL * ( NoV * ( 1 - a ) + a );
	float Vis_SmithL = NoV * ( NoL * ( 1 - a ) + a );

	return 0.5 * (1 / (Vis_SmithV + Vis_SmithL));

}

vec2 IntegrateBRDF(float Roughness, float NoV) {

	vec3 N = vec3(0, 0, 1);
	vec3 V = vec3(sqrt(1 - NoV * NoV), 0, NoV);
	vec2 AB = vec2(0);

	for (uint i = 0U; i < NUM_SAMPLES; i++) {

		vec2 Xi = Hammersley(i, NUM_SAMPLES);
		//vec3 H = TangentToWorld(GGXSample(Xi, Roughness), N);
		vec3 H = GGXSample(Xi, Roughness);

		vec3 L = 2 * dot(V, H) * H - V;

		float NoL = saturate(L.z);
		float NoH = saturate(H.z);
		float VoH = saturate(dot(V, H));

		if (NoL > 0) {

			float G = Vis_Smith(Roughness, NoV, NoL);
			//float G = Vis_SmithJointApprox(Roughness, NoV, NoL);
			float G_Vis = saturate(G * VoH / (NoH * NoV));

			float Fc = pow(1 - VoH, 5);

			AB += vec2(1 - Fc, Fc) * G_Vis;

		}

	}

	AB /= NUM_SAMPLES;

	return AB;
}

void main() {

	//vec2 ndc = gl_FragCoord.xy / 256;
	vec2 ndc = vs_out_position * 0.5 + 0.5;

	out_color.rg = IntegrateBRDF(ndc.y, ndc.x);

}
