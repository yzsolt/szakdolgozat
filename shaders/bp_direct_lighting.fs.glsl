
#version 330 core

layout(location = 0) out vec4 out_color;

in vec3 vs_out_world_position;
in vec3 vs_out_world_normal;
in vec2 vs_out_texture;
in vec3 vs_out_view_direction;

in mat3 vs_out_tbn;

#include "bp_pb_common.glsl"

uniform int u_light_type;

uniform PointLight u_point_light;
uniform SpotLight u_spot_light;

uniform BlinnPhongMaterial u_bpm;

/*
float get_angle_attenuation(vec3 l, SpotLight spot_light) {

    float current_angle = dot(-l, spot_light.direction);
    float inner_minus_outer = spot_light.inner_cone_angle - spot_light.outer_cone_angle;

    return clamp((current_angle - spot_light.outer_cone_angle) / inner_minus_outer, 0, 1);
}*/

float get_angle_attenuation(vec3 l, SpotLight spot_light) {

    float angle_scale = 1 / max(0.001f, spot_light.inner_cone_angle - spot_light.outer_cone_angle);
    float angle_offset = -spot_light.outer_cone_angle * angle_scale;

	float cosa = -dot(l, spot_light.direction);
	float attenuation = clamp(cosa * angle_scale + angle_offset, 0, 1);

	return attenuation * attenuation;

}

void main() {

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

    vec3 normal;
    vec3 light_direction = normalize(vs_out_world_position - position);

    if (u_bpm.normal.use_texture) {

        vec3 normal_map_value = normalize(texture(u_bpm.normal.texture, vs_out_texture).rgb * 2 - 1);
        normal = normalize(vs_out_tbn * normal_map_value);

    } else {
        normal = normalize(vs_out_world_normal);
    }

    float lambertian = max(dot(-light_direction, normal), 0);
    float specular = 0;

    float attenuation = 1;

    if (u_light_type == SPOT_LIGHT) {
        attenuation = get_angle_attenuation(light_direction, u_spot_light);
    }

    if (lambertian > 0) {

        vec3 half_direction = normalize(light_direction + vs_out_view_direction);
        float specular_angle = max(dot(half_direction, normal), 0);
        specular = pow(specular_angle, u_bpm.shininess);

    }

    vec4 diffuse_color = u_bpm.diffuse.use_texture ? texture(u_bpm.diffuse.texture, vs_out_texture) : u_bpm.diffuse.color;

    float specularity = u_bpm.specular.use_texture ? texture(u_bpm.specular.texture, vs_out_texture).r : 1;
    vec4 specular_component = specular * specularity * vec4(color, 1);

    float brightness = luminous_intensity / 20.f;

    vec4 blinn_phong_color = lambertian * diffuse_color * vec4(color, 1) * attenuation + specular_component * attenuation;
    blinn_phong_color *= brightness;

    out_color = vec4(blinn_phong_color.rgb, 1);

}
