
#version 330 core

layout(location = 0) out vec3 out_color;

in vec2 vs_out_position;

#include "precompute_common.glsl"

#define NUM_SAMPLES	2048U

float G_Smith(float ndotl, float ndotv, float roughness) {
	float a = roughness * 0.5f + 0.5f;
	float a2 = a * a * a * a;

	float lambda_v = (-1 + sqrt(a2 * (1 - ndotl * ndotl) / (ndotl * ndotl) + 1)) * 0.5f;
	float lambda_l = (-1 + sqrt(a2 * (1 - ndotv * ndotv) / (ndotv * ndotv) + 1)) * 0.5f;

	return 1 / (1 + lambda_v + lambda_l);
}

vec2 IntegrateBRDF(float roughness, float NoV) {
	vec3 N = vec3(0.0f, 0.0f, 1.0f);
	vec3 V = vec3(sqrt(1.0f - NoV * NoV), 0.0f, NoV);
	vec3 H;
	vec3 L;
	vec2 Xi;
	vec2 AB = vec2(0);

	for (uint i = 0U; i < NUM_SAMPLES; ++i) {
		Xi = Hammersley(i, NUM_SAMPLES);
		H = GGXSample(Xi, roughness);

		L = 2.0f * dot(V, H) * H - V;

		float NoL = saturate(L.z);
		float NoH = saturate(H.z);
		float VoH = saturate(dot(V, H));

		if ( NoL > 0 ) {
			float Fc = pow(1 - VoH, 5.0f);

			// PDF = (D * NoH) / (4 * VoH)
			float G = G_Smith(NoL, NoV, roughness);
			float G_mul_pdf = saturate((G * VoH) / (NoV * NoH));

			AB.x += (1 - Fc) * G_mul_pdf;
			AB.y += Fc * G_mul_pdf;
		}
	}

	AB.x /= float(NUM_SAMPLES);
	AB.y /= float(NUM_SAMPLES);

	return AB;
}

void main() {

	vec2 ndc = vs_out_position * 0.5 + 0.5;// / 256.0f;

	out_color.rg = IntegrateBRDF(ndc.y, ndc.x);
	out_color.b = 1;

}
