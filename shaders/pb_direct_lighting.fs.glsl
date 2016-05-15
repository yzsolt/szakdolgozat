
#version 330 core

layout(location = 0) out vec4 out_color;

in vec2 vs_out_texture;
in vec3 vs_out_world_normal;
in vec3 vs_out_view_direction;

in vec3 vs_out_world_position;

#include "pbr_common.glsl"

#define POINT_LIGHT 0
#define SPOT_LIGHT  1

uniform int u_light_type;

struct Light {
    vec3 position;
    vec3 color;
    float luminance;
    float luminous_intensity;
    float inverse_radius;
};

struct PointLight {
    Light parent;
};

uniform PointLight u_point_light;

struct SpotLight {
    Light parent;
    vec3 direction;
    float inner_cone_angle;
    float outer_cone_angle;
};

uniform SpotLight u_spot_light;

float get_angle_attenuation(vec3 l, SpotLight spot_light) {

    float angle_scale = 1 / max(0.001f, spot_light.inner_cone_angle - spot_light.outer_cone_angle);
    float angle_offset = -spot_light.outer_cone_angle * angle_scale;

	float cosa = -dot(l, spot_light.direction);
	float attenuation = clamp(cosa * angle_scale + angle_offset, 0, 1);

	return attenuation * attenuation;

}

void main() {

    float roughness = u_pbm.roughness.use_texture ? texture(u_pbm.roughness.texture, vs_out_texture).r : u_pbm.roughness.color.r;

    vec3 position;
    vec3 color;
    float luminance;
    float luminous_intensity;
    float inverse_radius;

    switch (u_light_type) {

        case POINT_LIGHT:
            position = u_point_light.parent.position;
            color = u_point_light.parent.color;
            luminance = u_point_light.parent.luminance;
            luminous_intensity = u_point_light.parent.luminous_intensity;
            inverse_radius = u_point_light.parent.inverse_radius;
            break;

        case SPOT_LIGHT:
            position = u_spot_light.parent.position;
            color = u_spot_light.parent.color;
            luminance = u_spot_light.parent.luminance;
            luminous_intensity = u_spot_light.parent.luminous_intensity;
            inverse_radius = u_spot_light.parent.inverse_radius;
            break;

    }

	vec3 n = normalize(vs_out_world_normal);
	vec3 v = normalize(vs_out_view_direction);
    vec3 light_direction = position - vs_out_world_position;

    vec3 l = normalize(light_direction);

	vec3 h = normalize(v + l);

	float ndotv = clamp(dot(n, v), 0, 1);
	float ndotl = clamp(dot(n, l), 0, 1);
	float ndoth = clamp(dot(n, h), 0, 1);
	float ldoth = clamp(dot(l, h), 0, 1);

	vec4 fd = BRDF_Lambertian(vs_out_texture);
	vec3 fs = BRDF_CookTorrance(ldoth, ndoth, ndotv, ndotl, roughness);

	float dist		= length(light_direction);
	float dist2		= max(dot(light_direction, light_direction), 1e-4);
	float falloff   = (luminous_intensity / dist2) * max(0, 1 - dist * inverse_radius);

    if (u_light_type == SPOT_LIGHT) {
        falloff *= get_angle_attenuation(l, u_spot_light);
    }

	float fade		= max(0, (fd.a - 0.75) * 4);
	float shadow	= mix(1, falloff, fade);

	vec3 final_color = (fd.rgb * fd.a + fs) * ndotl * shadow * color;

	out_color.rgb = final_color;
	out_color.a = fd.a;

}
