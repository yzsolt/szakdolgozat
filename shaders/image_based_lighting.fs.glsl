
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

vec3 approximate_specular(vec3 specular_color, float roughness, vec3 n, vec3 v) {

	float n_dot_v = clamp(dot(n, v), 0, 1);
	vec3 r = 2 * dot(v, n) * n - v;

	float mip_level = roughness * (NUM_MIPS - 1);

	vec3 specular_radiance = textureLod(u_specular_irradiance_map, r, mip_level).rgb;
	//vec3 specular_radiance = texture(u_specular_irradiance_map, r).rgb;
	vec2 brdf = texture(u_brdf_lut, vec2(n_dot_v, roughness)).rg;

	return specular_radiance * (specular_color * brdf.x + vec3(brdf.y));

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
	roughness = 1 - roughness;
    float metalness = u_pbm.metalness.use_texture ? texture(u_pbm.metalness.texture, vs_out_texture).r : u_pbm.metalness.color.r;

	vec4 fd					= BRDF_Lambertian(vs_out_texture);
	vec3 diffuse_rad		= texture(u_diffuse_irradiance_map, n).rgb * fd.rgb;

	vec3 specular_input = mix(vec3(0.04), diffuse_color.rgb, metalness);
	vec3 specular_output = approximate_specular(specular_input, roughness, n, v);

	out_color.rgb = diffuse_rad * fd.a + specular_output;
	out_color.a = fd.a;

	//out_color = vec4(normalize(u_view_position) * 0.5 + 0.5, 1);

}
