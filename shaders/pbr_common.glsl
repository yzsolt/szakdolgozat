
#define ONE_OVER_PI	0.318309
#define PI			3.1415926535897932
#define TWOPI		6.2831853071795864
#define HALF_PI     1.57
#define NUM_MIPS	9

#include "bp_pb_common.glsl"

uniform PhysicallyBasedMaterial u_pbm;

vec3 F_Schlick(vec3 f0, float u) {
	return f0 + (vec3(1.0) - f0) * pow(1.0 - u, 5.0);
}

float D_GGX(float ndoth, float roughness) {

	float m = roughness * roughness;
	float m2 = m * m;
	float d = (ndoth * m2 - ndoth) * ndoth + 1.0;

	return m2 / max(PI * d * d, 1e-8);

}

float Vis_Schlick(float ndotl, float ndotv, float roughness) {
	// = G_Schlick / (4 * ndotv * ndotl)
	float a = roughness + 1.0;
	float k = a * a * 0.125;

	float Vis_SchlickV = ndotv * (1 - k) + k;
	float Vis_SchlickL = ndotl * (1 - k) + k;

	return 0.25 / (Vis_SchlickV * Vis_SchlickL);

}

vec4 BRDF_Lambertian(vec2 tex) {

	vec4 color = u_pbm.diffuse.use_texture ? texture(u_pbm.diffuse.texture, vs_out_texture) : u_pbm.diffuse.color;
    color.a = 1;
    //vec4 color = mix(baseColor, albedo, matParams.z);

    float metalness = u_pbm.metalness.use_texture ? texture(u_pbm.metalness.texture, vs_out_texture).r : u_pbm.metalness.color.r;

	color.rgb = mix(color.rgb, vec3(0.0), metalness);
	color.rgb *= ONE_OVER_PI;

	return color;

}

vec3 BRDF_CookTorrance(float ldoth, float ndoth, float ndotv, float ndotl, float roughness) {

    vec4 diffuse_color = u_pbm.diffuse.use_texture ? texture(u_pbm.diffuse.texture, vs_out_texture) : u_pbm.diffuse.color;
    float metalness = u_pbm.metalness.use_texture ? texture(u_pbm.metalness.texture, vs_out_texture).r : u_pbm.metalness.color.r;

	vec3 F0 = mix(vec3(0.04), diffuse_color.rgb, metalness);
	vec3 F = F_Schlick(F0, ldoth);

	float Vis = Vis_Schlick(ndotl, ndotv, roughness);
	float D = D_GGX(ndoth, roughness);

	return F * Vis * D;

}

vec3 CosineSample(float xi1, float xi2)
{
    float phi = 2 * PI * xi1;
    float costheta = sqrt(1 - xi2);                  // mert P(θ) = 1 - cos2 θ
    float sintheta = sqrt(1 - costheta * costheta);  // mert sin2 x + cos2 x = 1

    // PDF = costheta / PI
    return vec3(sintheta * cos(phi), sintheta * sin(phi), costheta);
}

vec3 GGXSample(float xi1, float xi2, float roughness)
{
    float alpha2 = pow(roughness, 4.0); // eml.: Disney-féle α = roughness2

    float phi = 2 * PI * xi1;
    float costheta = sqrt((1 - xi2) / (1 + (alpha2 - 1) * xi2));
    float sintheta = sqrt(1 - costheta * costheta);

    // PDF = (D(h) * dot(n, h)) / (4 * dot(v, h))
    return vec3(sintheta * cos(phi), sintheta * sin(phi), costheta);
}

/*
float VarianceShadow2D(sampler2D shadowmap, vec4 cpos, vec2 nf)
{
	vec2 projpos = (cpos.xy / cpos.w) * 0.5 + 0.5;
	vec2 moments = texture(shadowmap, projpos).xy;

	float d01		= (cpos.z * 0.5 + 0.5);
	float z			= ((cpos.w < 0.0) ? -cpos.w : d01);
	float d			= (z - nf.x) / (nf.y - nf.x);
	float mean		= moments.x;
	float variance	= max(moments.y - moments.x * moments.x, 1e-5);
	float md		= mean - d;
	float pmax		= variance / (variance + md * md);

	pmax = smoothstep(0.3, 1.0, pmax);

	return max(d <= mean ? 1.0 : 0.0, pmax);
}
*/
