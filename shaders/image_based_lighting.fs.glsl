
#version 330 core

layout(location = 0) out vec4 out_color;

uniform samplerCube u_diffuse_irradiance_map;
uniform samplerCube u_specular_irradiance_map;
uniform sampler2D u_brdf_lut;

uniform vec3 u_view_position;

in vec2 vs_out_texture;
in vec3 vs_out_world_normal;
in vec3 vs_out_view_direction;

in mat3 vs_out_tbn;

#include "pbr_common.glsl"

vec2 approximate_brdf_ue4(vec3 SpecularColor, float Roughness, float NoV) {

	// [ Lazarov 2013, "Getting More Physical in Call of Duty: Black Ops II" ]
	// Adaptation to fit our G term.
	const vec4 c0 = vec4(-1, -0.0275, -0.572, 0.022);
	const vec4 c1 = vec4(1, 0.0425, 1.04, -0.04);
	vec4 r = Roughness * c0 + c1;
	float a004 = min( r.x * r.x, exp2( -9.28 * NoV ) ) * r.x + r.y;
	vec2 AB = vec2( -1.04, 1.04 ) * a004 + r.zw;

	//return SpecularColor * AB.x + AB.y;
	return AB;
}

vec2 approximate_brdf_knarkowicz(vec3 specularColor, float gloss, float ndotv ) {

    float x = gloss;
    float y = ndotv;

    float b1 = -0.1688;
    float b2 = 1.895;
    float b3 = 0.9903;
    float b4 = -4.853;
    float b5 = 8.404;
    float b6 = -5.069;
    float bias = clamp( min( b1 * x + b2 * x * x, b3 + b4 * y + b5 * y * y + b6 * y * y * y ), 0, 1 );

    float d0 = 0.6045;
    float d1 = 1.699;
    float d2 = -0.5228;
    float d3 = -3.603;
    float d4 = 1.404;
    float d5 = 0.1939;
    float d6 = 2.661;
    float delta = clamp( d0 + d1 * x + d2 * y + d3 * x * x + d4 * x * y + d5 * y * y + d6 * x * x * x, 0, 1 );
    float scale = delta - bias;

    return vec2(scale, bias);

}

vec3 approximate_specular(vec3 specular_color, float roughness, vec3 n, vec3 v) {

	float n_dot_v = dot(n, v);
	vec3 r = 2 * n_dot_v * n - v;

	float mip_level = roughness * (NUM_MIPS - 1);

	vec3 specular_radiance = textureLod(u_specular_irradiance_map, r, mip_level).rgb;
	vec2 brdf = texture(u_brdf_lut, vec2(clamp(n_dot_v, 0, 1), roughness)).rg;
	//vec2 brdf = approximate_brdf_ue4(specular_color, roughness, n_dot_v);
	//vec2 brdf = approximate_brdf_knarkowicz(specular_color, roughness, n_dot_v);

	// Anything less than 2% is physically impossible and is instead considered to be shadowing
	return specular_radiance * (specular_color * brdf.x + clamp(50 * specular_color.g, 0, 1) * vec3(brdf.y));

}

void main() {

	vec3 n;

	if (u_pbm.normal.use_texture) {

        vec3 normal_map_value = normalize(texture(u_pbm.normal.texture, vs_out_texture).rgb * 2 - 1);
        n = normalize(vs_out_tbn * normal_map_value);

    } else {
        n = normalize(vs_out_world_normal);
    }

	vec3 v = normalize(vs_out_view_direction);

    vec4 diffuse_color = u_pbm.diffuse.use_texture ? texture(u_pbm.diffuse.texture, vs_out_texture) : u_pbm.diffuse.color;
    float roughness = u_pbm.roughness.use_texture ? texture(u_pbm.roughness.texture, vs_out_texture).r : u_pbm.roughness.color.r;
    float metalness = u_pbm.metalness.use_texture ? texture(u_pbm.metalness.texture, vs_out_texture).r : u_pbm.metalness.color.r;

	vec4 fd				= BRDF_Lambertian(vs_out_texture);
	vec3 diffuse_rad	= texture(u_diffuse_irradiance_map, n).rgb * fd.rgb;

	vec3 specular_input = mix(vec3(0.04), diffuse_color.rgb, metalness);
	vec3 specular_output = approximate_specular(specular_input, roughness, n, v);

	out_color.rgb = diffuse_rad * fd.a + specular_output;
	out_color.a = fd.a;

}
